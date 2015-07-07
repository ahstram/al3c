class SMC_t {

public:

	void swap_frameworks() {

		framework_t<param_t> **tmp=last;
		last=current;
		current=tmp;

	}

	const char *dlsym_error;
	void *handle;

	create_t *user_type;
	create_summary_t *user_summary_type;

	destroy_t *destroy_user_type;
	destroy_summary_t *destroy_user_summary_type;

	framework_t<param_t> **last, **current, **proposed;
	param_t **last_params;
	framework_summary_t<param_summary_t> *last_summary;


	struct timers_t { double begin,end; } timers;

	uint d,D, n,N,g,G,T, T_per_proc,A, A_per_proc, *R, cheat, sample;

	float **O,*E, last_epsilon, current_epsilon, terminal_epsilon,x,sum_weight, max_weight;
	char *output_prefix, *ptr_a,*ptr_b,*ptr_c,*line, *O_string;

	char *last_data, *current_data, *proposed_data;

	uint size_of_mem;

	void update_last_params() {
		for (uint a=0;a<A;a++)
			last_params[a]=last[a]->param;
	}
	void sort_proposed() {
			if (np==0) {
				sort(proposed,proposed+T,sortMethod); 
				current_epsilon=*(proposed[A-1]->d);

				for (uint a=0,t=0;t<T && a<A;t++)
					if (*(proposed[t]->d) <= current_epsilon) {
						memcpy(current[a]->d,proposed[t]->d,size_of_mem);
						a++;
					} 

				current_epsilon=last_epsilon;
				if (G==0) {
					if (R[0])
						current_epsilon=*(current[R[0]-1]->d);
				} else {
					if (R[g-1])
						current_epsilon=*(current[R[g-1]-1]->d); 
					else
						current_epsilon=E[g-1];
				}

			current_epsilon=MAX(current_epsilon,terminal_epsilon);
			} MPI::COMM_WORLD.Bcast(&current_epsilon,1,MPI::FLOAT,0);
			MPI::COMM_WORLD.Bcast(current[0]->d,size_of_mem*A,MPI::CHAR,0);
	}	

	void calculate_weights() {

			float perturb_scale,perturb_scales=-FLT_MAX,prior_scale,prior_scales=-FLT_MAX,  *perturb_density_matrix=new float[A*A], *prior_density_vector=new float[A*A];

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

			delete [] perturb_density_matrix;
			delete [] prior_density_vector;
	}

	void print_summary() {

		if (np==0) {
			ostringstream s_output;
			s_output<<output_prefix<<g;

			ofstream f_output;
			f_output.open(s_output.str().data(),ios::out);

			assert(f_output.is_open());

			current[0]->print(f_output,1);

			for (uint a=0;a<A;a++) {
				current[a]->print(f_output,0);
			} f_output.close();

			ostringstream s_output_summary;
			s_output_summary<<output_prefix<<"summary";
			f_output.open(s_output_summary.str().data(),ios::out | std::ofstream::app);
			assert(f_output.is_open());
	
			if (g==1)
				f_output<<"generation\tepsilon\ttime"<<endl;
			f_output<<g<<"\t"<<last_epsilon<<"\t"<<timers.end-timers.begin<<endl;
		}
	}


	void print_progress(uint *_t) {

		if (np==0) {
			cerr<<"\rgeneration="<<g<<"/"<<G<<", epsilon="<<current_epsilon<<", simulations=("<<_t[0]<<"/"<<T_per_proc;
			for (uint r=1;r<NP;r++) {
				cerr<<","<<_t[r]<<"/"<<T_per_proc;
			}
			cerr<<")...";
		}
	}

	uint set_size_of_mem () {

		framework_t<param_t> *tmp_t=user_type(NULL,NULL,0,0,NULL);
				// d		w		param						S
		size_of_mem=sizeof(float)+sizeof(float)+tmp_t->size_of_param_t+(N)*(D)*sizeof(float);
	
		destroy_user_type(tmp_t);
	
		return size_of_mem;

	}


	void generate() {

		uint sample;

		uint *t=new uint[NP]();

		if(cheat && g>1) {
			for (uint i=0;i<A_per_proc && t[np]<T_per_proc;i++) {
				sample=np*A_per_proc+i;
				if (*(last[sample]->d)<=current_epsilon) {
					memcpy(proposed[np*T_per_proc+t[np]]->d,last[sample]->d,size_of_mem);
					t[np]++;
				}
			}
		}

		print_progress(t);
	
		for (;t[np]<T_per_proc;t[np]++) {

			repeat: 

			while (u01()*max_weight > *(last[sample=(uint)(u01()*(A-1))]->w))
				;

			retry:

			memcpy(proposed[np*T_per_proc+t[np]]->d,last[sample]->d,size_of_mem);

			proposed[np*T_per_proc+t[np]]->perturb();


			if (proposed[np*T_per_proc+t[np]]->prior_density()==0 || proposed[np*T_per_proc+t[np]]->perturb_density(last[sample]->param)==0)
				goto retry;

			proposed[np*T_per_proc+t[np]]->simulate();


			if ( (*(proposed[np*T_per_proc+t[np]]->d)=proposed[np*T_per_proc+t[np]]->distance() ) > current_epsilon) 
				goto repeat;

		}

		//one last print...
		print_progress(t);

		MPI_Barrier(MPI_COMM_WORLD);

		delete [] t;
	}

//enter our loop of SMC...
	void loop() {
	
		for (uint a=0;a<A_per_proc;a++) {
			last[np*A_per_proc+a]->prior();
			*(last[np*A_per_proc+a]->w)=1/(float)A;
			*(last[np*A_per_proc+a]->d)=-1.f; // let -1 mean "not simulated"
		}
		for (uint r=0;r<NP;r++) 
			MPI::COMM_WORLD.Bcast(last[A_per_proc*r]->d,size_of_mem*A_per_proc,MPI::CHAR,r);
	
	
		
		uint *t0=new uint[NP]();
		
		for (g=1;g<=G || G==0 ;g++) {
	
			timers.begin=MPI_Wtime();
	
			print_progress(t0);

			update_last_params(); 
		
			last_summary->summarize();
	
			max_weight=calc_max_weight(last,A_per_proc,np);
	
		//simplest fix for sorting problem is to just reorganize the pointers here... how much time am i wasting?
		
			for (uint t=0;t<T;t++) {
				destroy_user_type(proposed[t]);
				proposed[t]=user_type(proposed_data+size_of_mem*t,last_summary->summary,N,D,O);
			}
		
	
			generate();
	
			for (uint r=0;r<NP;r++)  //can't point to ->d because that won't necessarily be at the start (since we sorted the pointers)
				MPI::COMM_WORLD.Bcast(proposed_data+T_per_proc*r*size_of_mem,T_per_proc*size_of_mem,MPI::CHAR,r);

			last_epsilon=current_epsilon;


			sort_proposed();
	// calc	ulate perturb_density_matrix and prior_density_vector
			calculate_weights();

			timers.end=MPI_Wtime();
	
		//printing status...
	
			if (np==0)	
				cerr<<" in "<<(timers.end-timers.begin)<<" seconds"<<endl;
	
			print_summary();	
			
		//switch our "last" with "current" acceptances
			swap_frameworks(); //switch current & last
	
		//quit if the terminal_epsilon has been reached, otherwise continue
			if (last_epsilon<=terminal_epsilon) {
				if (np==0)
					cerr<<"quitting because our final epsilon threshold '"<<terminal_epsilon<<"' has been reached"<<endl;
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
	
			
	}
	
	SMC_t(rapidxml::xml_document<> *cfg) {

	
		d=0, D=0, n=0, cheat=0,cheat=0;
		last_epsilon=FLT_MAX, current_epsilon=FLT_MAX,x=0,sum_weight=0;

		output_prefix=strdup(cfg->first_node("output")->first_node("prefix")->value());

		if(!cfg->first_node("ABC")->first_node("T")) {
			if (np==0)
				cerr<<"Error! Could not find required <ABC><T></T></ABC> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}
		T=atoi(cfg->first_node("ABC")->first_node("T")->value());
	
		if(!cfg->first_node("ABC")->first_node("A")) {
			if (np==0)
				cerr<<"Error! Could not find required <ABC><A></A></ABC> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}
		A=atoi(cfg->first_node("ABC")->first_node("A")->value());
			
	// reset T/A so that NP divides them...
		if (np==0)
			cerr<<"Requested "<<T<<" trials and "<<A<<" acceptances,";
	
		T_per_proc=(T+NP-1)/NP;
		A_per_proc=(A+NP-1)/NP;
	
		T=T_per_proc*NP;
		A=A_per_proc*NP;

		if (np==0)
			cerr<<" giving you "<<T<<" and "<<A<<endl;
		
	
		if(!cfg->first_node("ABC")->first_node("G")) {
			if (np==0)
				cerr<<"Error! Could not find required <ABC><G></G></ABC> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}
		G=atoi(cfg->first_node("ABC")->first_node("G")->value());
	
		if(!cfg->first_node("ABC")->first_node("R")) {
			if (np==0)
				cerr<<"Error! Could not find required <ABC><R></R></ABC> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}
		if (G!=0)
			R=new uint[G];
		else
			R=new uint[1];
		stringstream ABC_R(cfg->first_node("ABC")->first_node("R")->value());
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
				cerr<<"Error! <ABC><R>"<<cfg->first_node("ABC")->first_node("R")->value()<<"</R></ABC> must have length 1 or G="<<G<<endl;
			exit(EXIT_FAILURE);
		}

		if(!cfg->first_node("ABC")->first_node("E")) {
			if (np==0)
				cerr<<"Error! Could not find required <ABC><E></E></ABC> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}
	
		if (G!=0) 
			E=new float[G];
		else
			E=new float[1];
		stringstream ABC_E(cfg->first_node("ABC")->first_node("E")->value());
		for (g=0;ABC_E.good();g++)  {
			ABC_E>>E[g];
		}
		if (g==1) {
			for (g=1;g<G;g++)
				E[g]=E[0];
		} else if (g!=G) {
			if (np==0)
				cerr<<"Error! <ABC><E>"<<cfg->first_node("ABC")->first_node("E")->value()<<"</E></ABC> must have length 1 or G="<<G<<endl;
			exit(EXIT_FAILURE);
		}
		if (G!=0)
			terminal_epsilon=E[G-1];
		else
			terminal_epsilon=E[0];	
		
		if(!cfg->first_node("O")) {
			if (np==0)
				cerr<<"Error! Could not find required <O></O> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}

		O_string=strdup(cfg->first_node("O")->value());
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
		O_string=cfg->first_node("O")->value();
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
		
		if(cfg->first_node("ABC")->first_node("cheat"))
			cheat=atoi(cfg->first_node("ABC")->first_node("cheat")->value());
	
	
		//use dlopen to open the user provided library & check we have what we need
	
	
	
		if (np==0) {
			cerr<<"Observed data is a(n) "<<N<<"x"<<D<<" matrix, as follows:"<<endl<<endl;
			for (n=0;n<N;n++) {
				cerr<<O[n][0];
				for (d=1;d<D;d++)
					cerr<<"\t"<<O[n][d];
				cerr<<endl;	
			} cerr<<endl; 
		}

		if(!cfg->first_node("lib")) {
			cerr<<"Error! Could not find required <lib></lib> in XML file"<<endl;
			exit(EXIT_FAILURE);
		}

		char *lib_string=cfg->first_node("lib")->value();

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
	
		destroy_user_summary_type=(destroy_summary_t*)dlsym(handle,"destroy_summary");
		dlsym_error = dlerror();
		if (dlsym_error) {
			cerr << "Cannot load symbol: '" << dlsym_error << "'\n";
			exit(EXIT_FAILURE);
    		}
	
		if (np==0)
			cerr<<" done"<<endl;
	
		last=new framework_t<param_t>*[A], current=new framework_t<param_t>*[A], proposed=new framework_t<param_t>*[T];
	
		last_params=new param_t*[A];
	
		last_summary=user_summary_type(last_params,A);
	
		set_size_of_mem();
	
		last_data=new char[size_of_mem*A](), current_data=new char[size_of_mem*A](), proposed_data=new char[size_of_mem*T]() ;
	
		for (uint t=0;t<T;t++) {
			proposed[t]=user_type(proposed_data+size_of_mem*t,last_summary->summary,N,D,O);
			if (t<A) {
				current[t]=user_type(current_data+size_of_mem*t,last_summary->summary,N,D,O);
				last[t]=user_type(last_data+size_of_mem*t,last_summary->summary,N,D,O);	
			}
		} 
	}
	
	~SMC_t() {
	
		dlclose(handle);

		free(output_prefix);

		delete [] E;
		delete [] R;
		destroy_user_summary_type(last_summary);
	
		for (uint t=0;t<T;t++) {
			destroy_user_type(proposed[t]);
			if (t<A) {
				destroy_user_type(current[t]);
				destroy_user_type(last[t]);
			}
		} 
		for (uint i=0;i<N;i++)
			delete [] O[i];
		delete [] O;
	
		delete [] proposed;
		delete [] current;
		delete [] last;
		delete [] last_params;
	
		delete [] last_data;
		delete [] current_data;
		delete []  proposed_data;
	
	}


};
