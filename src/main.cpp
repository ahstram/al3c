#define VERSION	15.7
#define AL3C 1
#define MAX(a,b)	((a)>(b) ? (a):(b)) 
#define MIN(a,b)	((a)<(b) ? (a):(b)) 
#define DELIM   "\t ," 
#define LINE_LEN	1024 
#define __STDC_LIMIT_MACROS

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <algorithm> //for sort
#include <sstream>
#include <vector>
#include <time.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include <float.h>
#include <time.h>
#include <iomanip>
#include <stdint.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "rapidxml/rapidxml.hpp"

#include "../include/al3c.hpp"

int np;

struct timers_t {
	double begin,end;
} timers;

#include "u01.cpp"
#include "progress.cpp"
#include "weight.cpp"
#include "generate.cpp"
#include "mpi_check.cpp"
#include "signal.cpp"

int main (int argc, char *argv[] ) {


//configuration file...
	if (argc!=2) {
		cerr<<"Error! Run with "<<argv[0]<<" <XML configuration>"<<endl;
		exit(EXIT_FAILURE);
	}

	ifstream xmlfile (argv[1]);
	
	if (!xmlfile) {
		cerr<<"Error! Could not open XML file '"<<argv[1]<<"'"<<endl;
		exit(EXIT_FAILURE);
	}

//variables
	
	uint NP_XML=0,NP=0;

	uint d=0,D=0, n=0,N,g=0,G,T, T_per_proc,A, A_per_proc, *R, cheat=0, sample;
	float **O=NULL,*E, last_epsilon=FLT_MAX, epsilon=FLT_MAX,x=0,sum_weight=0, max_weight, perturb_scale,perturb_scales,prior_scale,prior_scales,  *perturb_density_matrix, *prior_density_vector, quit_threshold;

	char *output_prefix=NULL, *lib_string=NULL, *ptr_a,*ptr_b,*ptr_c,*line, *O_string=NULL, *last_data, *current_data, *proposed_data;

	const char *dlsym_error;
	void *handle;

	create_t *user_type;
	create_summary_t *user_summary_type;

	destroy_t *destroy_user_type;
	destroy_summary_t *destroy_user_summary_type;

	framework_t<param_t> **last, **current, **proposed,  **tmp;
	param_t **last_params;
	uint size_of_mem;
	
	framework_summary_t<param_summary_t> *last_summary;


//register signal handler
	signal(SIGINT, signal_callback_handler);


//get MPI running...
	MPI::Init();

	np=MPI::COMM_WORLD.Get_rank(),  NP=MPI::COMM_WORLD.Get_size();
//make sure only one argument specified
	if (argc!=2) {
		cerr<<"Error! Incorrect arguments specified. Usage is: 'al3c config.xml'"<<endl;
		exit(EXIT_FAILURE);
	}

//if not already running in MPI, this will invoke it for us...

	vector<char> buffer((istreambuf_iterator<char>(xmlfile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	using namespace rapidxml;
	xml_document<> doc;    // character type defaults to char
	doc.parse<0>(&buffer[0]);    // 0 means default parse flags


	if (!doc.first_node("MPI")->first_node("NP")) {
		cerr<<"Error! Could not find required <MPI><NP></NP></MPI> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
		
	NP_XML=atoi(doc.first_node("MPI")->first_node("NP")->value());

	if (NP_XML!=NP && NP==1) {

		int returncode;

		char **args=new char*[4+argc];
		args[0]=strdup("mpirun");
		args[1]=strdup("-np");
		args[2]=strdup(doc.first_node("MPI")->first_node("NP")->value());

		for (int i=0;i<argc;i++) {
			args[3+i]=argv[i];
		} args[3+argc]=NULL; 

		if (check_mpirun())
			returncode=execvp("mpirun",args);
		else
			returncode=execvp("bin/mpirun",args); 

		if (returncode!=0) 
			cerr<<"Warning: mpirun exited with code '"<<returncode<<"'"<<endl;

		for (int i=0;i<3;i++)
			free(args[i]);
		delete [] args;

		exit(returncode);
	} 

	if (np==0)
		cerr<<"al3c version "<<VERSION<<" initialized with "<<NP<<" processors"<<endl;

	print_cpu_info();

	output_prefix=strdup(doc.first_node("output")->first_node("prefix")->value());

	if(!doc.first_node("ABC")->first_node("T")) {
		if (np==0)
			cerr<<"Error! Could not find required <ABC><T></T></ABC> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
	T=atoi(doc.first_node("ABC")->first_node("T")->value());

	if(!doc.first_node("ABC")->first_node("A")) {
		if (np==0)
			cerr<<"Error! Could not find required <ABC><A></A></ABC> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
	A=atoi(doc.first_node("ABC")->first_node("A")->value());
	if(!doc.first_node("ABC")->first_node("G")) {
		if (np==0)
			cerr<<"Error! Could not find required <ABC><G></G></ABC> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
	G=atoi(doc.first_node("ABC")->first_node("G")->value());

	if(!doc.first_node("ABC")->first_node("R")) {
		if (np==0)
			cerr<<"Error! Could not find required <ABC><R></R></ABC> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
	if (G!=0)
		R=new uint[G];
	else
		R=new uint[1];
	stringstream ABC_R(doc.first_node("ABC")->first_node("R")->value());
	for (g=0;ABC_R.good();g++)  {
		ABC_R>>R[g];
		if (A<R[g]) {
			if(np==0)
				cerr<<"Error! <ABC><R>"<<R[g]<<"</R></ABC> must between 0 and A="<<A<<endl;
			exit(EXIT_FAILURE);
		}
	}
	if (g==1)
		for (g=1;g<G;g++)
			R[g]=R[0];
	else if (g!=G) {
		if (np==0)
			cerr<<"Error! <ABC><R>"<<doc.first_node("ABC")->first_node("R")->value()<<"</R></ABC> must have length 1 or G="<<G<<endl;
		exit(EXIT_FAILURE);
	}



	if(!doc.first_node("ABC")->first_node("E")) {
		if (np==0)
			cerr<<"Error! Could not find required <ABC><E></E></ABC> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}

	if (G!=0) 
		E=new float[G];
	else
		E=new float[1];
	stringstream ABC_E(doc.first_node("ABC")->first_node("E")->value());
	for (g=0;ABC_E.good();g++)  {
		ABC_E>>E[g];
	}
	if (g==1) {
		for (g=1;g<G;g++)
			E[g]=E[0];
	} else if (g!=G) {
		if (np==0)
			cerr<<"Error! <ABC><E>"<<doc.first_node("ABC")->first_node("E")->value()<<"</E></ABC> must have length 1 or G="<<G<<endl;
		exit(EXIT_FAILURE);
	}
	if (G!=0)
		quit_threshold=E[G-1];
	else
		quit_threshold=E[0];	
	
	if(!doc.first_node("O")) {
		if (np==0)
			cerr<<"Error! Could not find required <O></O> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
	//O_string=new char[doc.first_node("O")->value_size()+1];

	//memcpy(O_string,doc.first_node("O")->value(),doc.first_node("O")->value_size()+1);
	O_string=strdup(doc.first_node("O")->value());
	line=strtok_r(O_string,"\n",&ptr_b);
	if (line[0]=='#')
		N=0;
	else {
		N=1;
		atof(strtok_r(line,DELIM,&ptr_c));
		D=1;

		while(strtok_r(NULL,DELIM,&ptr_c))
			D++;
	}
	while ((line=strtok_r(NULL,"\n",&ptr_b)))
		if (line[0]!='#') {
			if (N==1) {
				atof(strtok_r(line,DELIM,&ptr_c));
				D=1;
				while(strtok_r(NULL,DELIM,&ptr_c))
					D++;
			}
			N++;
		}

	O=new float*[N];
	free(O_string);
	O_string=doc.first_node("O")->value();
	line=strtok_r(O_string,"\n",&ptr_b);	
	n=0;
	while(1) {
		if (line[0]!='#') {
			O[n]=new float[D];
			O[n][0]=atof(strtok_r(line,DELIM,&ptr_c));
			for (d=1;(ptr_a=strtok_r(NULL,DELIM,&ptr_c))!=NULL;d++)
				O[n][d]=atof(ptr_a);
			n++;
		}
		if (!(line=strtok_r(NULL,"\n",&ptr_b)))
			break;
	}

	assert(n==N && d==D);
	
	if(doc.first_node("ABC")->first_node("cheat"))
		cheat=atoi(doc.first_node("ABC")->first_node("cheat")->value());


	//use dlopen to open the user provided library & check we have what we need

	if(!doc.first_node("lib")) {
		cerr<<"Error! Could not find required <lib></lib> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
	lib_string=doc.first_node("lib")->value();

	if (np==0) 
		cerr<<"Loading '"<<lib_string<<"' shared library...";

	handle= dlopen(lib_string,RTLD_LAZY);

	if (!handle) {
		cerr << "Cannot open library: " << dlerror() << '\n';
		exit(EXIT_FAILURE);
	}

	user_type=(create_t*)dlsym(handle,"create");
	dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol: '" << dlsym_error << "'\n";
		exit(EXIT_FAILURE);
	}

	destroy_user_type=(destroy_t*)dlsym(handle,"destroy");
	dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol: '" << dlsym_error << "'\n";
		exit(EXIT_FAILURE);
	}


	user_summary_type=(create_summary_t*)dlsym(handle,"create_summary");
	
	dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol: '" << dlsym_error << "'\n";
		exit(EXIT_FAILURE);
    	}

        // GKC: getting rid of compiler warning as destroy_summary_type variable
        // was not needed.
	//(destroy_summary_t*)dlsym(handle,"destroy_summary");
	destroy_user_summary_type=(destroy_summary_t*)dlsym(handle,"destroy_summary");
	dlsym_error = dlerror();
	if (dlsym_error) {
		cerr << "Cannot load symbol: '" << dlsym_error << "'\n";
		exit(EXIT_FAILURE);
    	}

// give the userr basic info about our simulation & reset T/A so that NP divides them...
	if (np==0)
		cerr<<" done\nRequested "<<T<<" trials and "<<A<<" acceptances,";
	

	T_per_proc=(T+NP-1)/NP;
	A_per_proc=(A+NP-1)/NP;

	T=T_per_proc*NP;
	A=A_per_proc*NP;


	if (np==0) {
		cerr<<" giving you "<<T<<" and "<<A<<endl;
		cerr<<"Observed data is a(n) "<<N<<"x"<<D<<" matrix, as follows:"<<endl<<endl;
		for (n=0;n<N;n++) {
			cerr<<O[n][0];
			for (d=1;d<D;d++)
				cerr<<"\t"<<O[n][d];
			cerr<<endl;	
		} cerr<<endl; 
	}

//get all our frameworks set up properly...
	
	last=new framework_t<param_t>*[A], current=new framework_t<param_t>*[A], proposed=new framework_t<param_t>*[T];

	last_params=new param_t*[A];

	last_summary=user_summary_type(last_params,A);


	framework_t<param_t> *tmp_t=user_type(NULL,NULL,0,0,NULL);

			// d		w		param						S
	size_of_mem=sizeof(float)+sizeof(float)+tmp_t->size_of_param_t+(N)*(D)*sizeof(float);

	destroy_user_type(tmp_t);

	last_data=new char[size_of_mem*A](), current_data=new char[size_of_mem*A](), proposed_data=new char[size_of_mem*T]() ;

	for (uint t=0;t<T;t++) {
		proposed[t]=user_type(proposed_data+size_of_mem*t,last_summary->summary,N,D,O);
		if (t<A) {
			current[t]=user_type(current_data+size_of_mem*t,last_summary->summary,N,D,O);
			last[t]=user_type(last_data+size_of_mem*t,last_summary->summary,N,D,O);
		}
	} 

//distribute our last parameters from the prior() distribution, weight properly, and broadcast 

	for (uint a=0;a<A_per_proc;a++) {
		last[np*A_per_proc+a]->prior();
		*(last[np*A_per_proc+a]->w)=1/(float)A;
		*(last[np*A_per_proc+a]->d)=-1.f; // let -1 mean "not simulated"
	}
	for (uint r=0;r<NP;r++) 
		MPI::COMM_WORLD.Bcast(last[A_per_proc*r]->d,size_of_mem*A_per_proc,MPI::CHAR,r);

	perturb_density_matrix=new float[A*A];
	prior_density_vector=new float[A*A];

//enter our loop of generations...

	uint *t0=new uint[NP]();

	for (g=1;g<=G || G==0 ;g++) {

		timers.begin=MPI_Wtime();

		print_progress(t0,T_per_proc,  NP,g, G, epsilon);

		for (uint a=0;a<A;a++)
			last_params[a]=last[a]->param;

		last_summary->summarize();


		max_weight=calc_max_weight(last,A_per_proc,np);

		//simplest fix for sorting problem is to just reorganize the pointers here... how much time am i wasting?
	
		for (uint t=0;t<T;t++) {
			destroy_user_type(proposed[t]);
			proposed[t]=user_type(proposed_data+size_of_mem*t,last_summary->summary,N,D,O);
		}
	
		generate(O,N,D,proposed+T_per_proc*np,T_per_proc, last, max_weight,  A_per_proc, A, epsilon,  NP, g, G, last_summary->summary,cheat, size_of_mem);

		for (uint r=0;r<NP;r++)  //can't point to ->d because that won't necessarily be at the start (since we sorted the pointers)
			MPI::COMM_WORLD.Bcast(proposed_data+T_per_proc*r*size_of_mem,T_per_proc*size_of_mem,MPI::CHAR,r);

		last_epsilon=epsilon;

		if (np==0) {
			sort(proposed,proposed+T,sortMethod); 
			epsilon=*(proposed[A-1]->d);

			for (uint a=0,t=0;t<T && a<A;t++)
				if (*(proposed[t]->d) <= epsilon) {
					memcpy(current[a]->d,proposed[t]->d,size_of_mem);
					a++;
				} 

			epsilon=last_epsilon;
			if (G==0) {
				if (R[0])
					epsilon=*(current[R[0]-1]->d);
			} else {
				if (R[g-1])
					epsilon=*(current[R[g-1]-1]->d); 
				else
					epsilon=E[g-1];
			}

		epsilon=MAX(epsilon,quit_threshold);
		} MPI::COMM_WORLD.Bcast(&epsilon,1,MPI::FLOAT,0);
		MPI::COMM_WORLD.Bcast(current[0]->d,size_of_mem*A,MPI::CHAR,0);

	// calculate perturb_density_matrix and prior_density_vector

		prior_scales=-FLT_MAX;
		perturb_scales=-FLT_MAX;
		for (uint a=0;a<A_per_proc;a++) {
			sample=np*A_per_proc+a;
			prior_scales=MAX(prior_density_vector[sample]=log(current[sample]->prior_density()),prior_scales);
			for (uint i=0;i<A;i++) 
				perturb_scales=MAX(perturb_density_matrix[sample*A+i]=log(current[sample]->perturb_density(last[i]->param)),perturb_scales);
		} 
		for (uint r=0;r<NP;r++) { //need to broadcast the above matrices..
			MPI::COMM_WORLD.Bcast(prior_density_vector+A_per_proc*r,A_per_proc,MPI::FLOAT,r);
			MPI::COMM_WORLD.Bcast(perturb_density_matrix+A_per_proc*A*r,A_per_proc*A,MPI::FLOAT,r);	
		} MPI::COMM_WORLD.Allreduce(&prior_scales,&prior_scale,1,MPI::FLOAT,MPI_MAX);
		MPI::COMM_WORLD.Allreduce(&perturb_scales,&perturb_scale,1,MPI::FLOAT,MPI_MAX);
	
		if (perturb_scale==0)
			perturb_scale=1;
		if (prior_scale==0)
			prior_scale=1;


	// assign our accepted weights..
		for (uint a=0;a<A_per_proc;a++) {
			sample=np*A_per_proc+a;
			x=0;
			for (uint i=0;i<A;i++) {
				x+=*(last[i]->w)*exp(perturb_density_matrix[sample*A+i]/perturb_scale);
			}
			*(current[sample]->w)=exp(prior_density_vector[sample]/prior_scale)/x;
		}
		sum_weight=calc_sum_weight(current,A_per_proc,np);
		for (uint a=0;a<A_per_proc;a++) {
			*(current[np*A_per_proc+a]->w)/=sum_weight;
		}
		for (uint r=0;r<NP;r++)
			MPI::COMM_WORLD.Bcast(current[A_per_proc*r]->d,size_of_mem*A_per_proc,MPI::CHAR,r);

		//switch our "last" with "current" acceptances

		tmp=last;
		last=current;
		current=tmp;

		timers.end=MPI_Wtime();

	//printing status...

		if (np==0)	
			cerr<<" in "<<(timers.end-timers.begin)<<" seconds"<<endl;

		print_summary(output_prefix,g,A,last_epsilon,current);	
			
	//quit if the quit_threshold has been reached, otherwise continue
		if (last_epsilon<=quit_threshold) {
			if (np==0)
				cerr<<"quitting because our final epsilon threshold '"<<quit_threshold<<"' has been reached"<<endl;
			break;
		}

	//if we've reached a CTRL+C, quit...
		if (SIGNUM) {
			if (np==0)
				cerr<<"quitting because signal '"<<SIGNUM<<"' has been received"<<endl;
			break;
		}

	}

	delete [] t0;

	delete [] perturb_density_matrix;
	delete [] prior_density_vector;



	delete [] R;
	destroy_user_summary_type(last_summary);

	for (uint t=0;t<T;t++) {
		destroy_user_type(proposed[t]);
		if (t<A) {
			destroy_user_type(current[t]);
			destroy_user_type(last[t]);
		}
	} 


	delete [] proposed;
	delete [] current;
	delete [] last;
	delete [] last_params;

	delete [] last_data;
	delete [] current_data;
	delete []  proposed_data;

	

	delete [] E;

	for (uint i=0;i<N;i++)
		delete [] O[i];
	delete [] O;

	free(output_prefix);

	delete [] rnd_array;
	// gracefully quit
	MPI::Finalize();

	dlclose(handle);

	exit(EXIT_SUCCESS);
}
