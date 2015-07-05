#define VERSION	15.7
#define AL3C 1
#define MAX(a,b)	((a)>(b) ? (a):(b)) 
#define MIN(a,b)	((a)<(b) ? (a):(b)) 
#define DELIM   "\t ," 
#define LINE_LEN	1024 
#define __STDC_LIMIT_MACROS

#include <iostream>
#include <algorithm> //for sort
#include <sstream>
#include <vector>
#include <time.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include <float.h>
#include <dlfcn.h>
#include <sys/stat.h>

#include "rapidxml/rapidxml.hpp"

#include "../include/al3c.hpp"

uint np=0, NP=0, SIGNUM=0;
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
	
	
//register signal handler
	signal(SIGINT, signal_callback_handler);


//get MPI running...
	MPI::Init();

	np=MPI::COMM_WORLD.Get_rank(),  NP=MPI::COMM_WORLD.Get_size();

//if not already running in MPI, this will invoke it for us...

	vector<char> buffer((istreambuf_iterator<char>(xmlfile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	rapidxml::xml_document<> doc;    // character type defaults to char
	doc.parse<0>(&buffer[0]);    // 0 means default parse flags


	if (!doc.first_node("MPI")->first_node("NP")) {
		cerr<<"Error! Could not find required <MPI><NP></NP></MPI> in XML file"<<endl;
		exit(EXIT_FAILURE);
	}
		

	if (atoi(doc.first_node("MPI")->first_node("NP")->value())!=(int)NP && NP==1) {

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

//initialize our ABC routine
	generations_t generations(&doc);

//begin the loop 
	generations.loop();
		
	delete [] rnd_array;
	// gracefully quit
	MPI::Finalize();

	exit(EXIT_SUCCESS);

}
