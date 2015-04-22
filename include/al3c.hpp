#include <fstream>


using namespace std;
typedef unsigned int uint;



struct param_t;
//class param_summary_t;

struct param_summary_t;
uint get_np();

template<typename param_t> class framework_t {

public:

	//these are constantly evolving & copied when parameters move...
	float *d, *w, **S;
	param_t *param;

	// these stay static and are just pointers to one location per processor	
	float **O;
	uint N, D;

	param_summary_t *param_summary;

	virtual void prior()=0;

	virtual float prior_density()=0;

	virtual void perturb()=0;	
	
	virtual float perturb_density(param_t *old_param)=0;

	virtual void simulate()=0;

	virtual float distance()=0;
	virtual void print(ofstream& output, bool header)=0;
	
	uint exec_cmd(const char *cmd) {

		char *output=new char[N*D*32];

		ostringstream new_cmd;
		#ifdef __linux
			new_cmd<<"taskset -c "<<sched_getcpu()<<" ";
		#endif
		new_cmd<<cmd<<endl;
		FILE *pipe=popen(new_cmd.str().c_str(),"r");

		//FILE *pipe=popen(cmd,"r");

	//	cerr<<"running:"<<new_cmd.str().c_str()<<endl;

		uint n,d=0;

		for (n=0;fgets(output,N*D*32,pipe) && n<N;n++) {
	       		S[n][0]=atof(strtok(output,"\t, "));
			for (d=1;d<D;d++)
				S[n][d]=atof(strtok(NULL,"\t, "));
		}

		pclose(pipe);

		delete output;

		if (n!=N || d!=D) {
			cerr<<"Warning! Command '"<<cmd<<"' did not give an "<<N<<"x"<<D<<" matrix"<<endl;
			return 1;
		}

		return 0;
	}
	uint size_of_param_t;

	#ifdef AL3C
		uint find_size_of_param_t();
	#else	
		uint find_size_of_param_t() {  return sizeof(param_t); }
	#endif


};

template<typename param_summary_t> class framework_summary_t {
	public:
	uint size_of_param_summary_t;
	param_summary_t *summary;

	param_t **params;
	uint A;

	virtual void summarize()=0;


	#ifdef AL3C
		uint find_size_of_param_summary_t();
	#else
		uint find_size_of_param_summary_t() { return sizeof(param_summary_t);}
	#endif
};

class user_summary_t:public framework_summary_t<param_summary_t> {

	public:
	user_summary_t(param_t **_params,uint _A) {
		params=_params;
		A=_A;
		size_of_param_summary_t=find_size_of_param_summary_t(); 

		summary=reinterpret_cast<param_summary_t*>(new char[size_of_param_summary_t]);

	}
	void summarize();
        virtual ~user_summary_t() {
        };

};

class user_t:public framework_t<param_t> {

	public:


 	void prior();
	float prior_density();
	void perturb();
	float perturb_density(param_t *old_param);
	void simulate();
	float distance();
	void print(ofstream& output,bool header);
	user_t(char *ptr, param_summary_t *ss_ptr,uint N_xml, uint D_xml, float **O_xml) { // this is our constructor, allocates properly...
		size_of_param_t=find_size_of_param_t();
		N=N_xml;
		D=D_xml;
		O=O_xml;
		param_summary=ss_ptr;

		d=reinterpret_cast<float *>(ptr); //ABSOLUTLEY MUST HAVE d AS THE FIRST...
		w=d+1;
		param=reinterpret_cast<param_t *>(w+1);		

		S=new float*[N]; //this is OK since it's just more pointers

		for (uint n=0;n<N;n++) 			// ptr 	d		w		param		..S...
			S[n]=reinterpret_cast<float *>( ptr+sizeof(float)+sizeof(float)+find_size_of_param_t()+(D)*sizeof(float)*n );
	}
	virtual ~user_t() {
		delete S;
	};

};  


// the types of the class factories
typedef framework_t<param_t>* create_t(char *ptr, param_summary_t *ss_ptr,uint N_xml, uint D_xml, float **O_xml);
typedef void destroy_t(framework_t<param_t>* user);
typedef framework_summary_t<param_summary_t>* create_summary_t(param_t **params, uint A);
typedef void destroy_summary_t(framework_summary_t<param_summary_t>* user_summary);

#ifndef	AL3C
extern "C" framework_t<param_t>* create(char *ptr, param_summary_t *ss_ptr,uint N_xml, uint D_xml, float **O_xml) {
	return new user_t(ptr,ss_ptr,N_xml,D_xml,O_xml);
}
extern "C" void destroy(user_t *user) {
	delete user;
}
extern "C" framework_summary_t<param_summary_t>* create_summary(param_t **params, uint A) {
	return new user_summary_t(params,A);
}
extern "C" void destroy_summary(user_summary_t *user_summary) {
	delete user_summary;
}

float u01();
float n01(float u1, float u2, bool use_sin);

#endif
