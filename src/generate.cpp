void generate(float **O, uint N, uint D,framework_t<param_t> **proposed, uint T, framework_t<param_t> **last, framework_t<param_t> *loan, float max_weight, uint A_per_proc, uint A, float eps, uint np, uint NP, uint g, uint G, param_summary_t *summary,bool cheat, uint size_of_mem) {

	uint sample;

	uint *t=new uint[NP]();
	t[np]=0;// this should be superfluous...
	
	//MPI_Status status;

	//int flag=0;
	//uint sum;

	//MPI_Request mpi_request[(T+1)*NP]; 

	if(cheat && g>1)
		for (uint i=0;i<A_per_proc && t[np]<T;i++) {
			sample=np*A_per_proc+i;
			if (*(last[sample]->d)<=eps) {
				memcpy(proposed[t[np]]->d,last[sample]->d,size_of_mem);
				t[np]++;
			}
		}


	//update_progress(t,T,np,NP,eps,mpi_request);

	print_progress(t,T,np,NP,g,G,eps);

	
	for (;t[np]<T;t[np]++) {

		repeat: 

		/*MPI_Iprobe(MPI_ANY_SOURCE,T*NP+g,MPI_COMM_WORLD,&flag,&status);
		if (flag) {
			MPI::COMM_WORLD.Recv(loan->d,size_of_mem,MPI::CHAR,status.MPI_SOURCE,status.MPI_TAG);
			memcpy(proposed[t[np]]->d,loan->d,size_of_mem);
			goto got_help;
		}*/
		

		
		while (u01()*max_weight > *(last[sample=(uint)(u01()*(A-1))]->w))
			;

		retry:

		memcpy(proposed[t[np]]->d,last[sample]->d,size_of_mem);

		proposed[t[np]]->perturb();


		if (proposed[t[np]]->prior_density()==0 || proposed[t[np]]->perturb_density(last[sample]->param)==0)
			goto retry;

		proposed[t[np]]->simulate();

		if ( (*(proposed[t[np]]->d)=proposed[t[np]]->distance() ) > eps)
			goto repeat;


	//END DIY SECTION

	//	got_help:
	//		;


		//update_progress(t,T,np,NP,eps,mpi_request);
	//	print_progress(t,T,np,NP,g,G,eps);

	}


/*
	//let all other processes know we're done...
        for (uint r=0;r<NP;r++)
                if (r!=np)
                        MPI::COMM_WORLD.Send(t+np,1,MPI::INT,r,t[np]);  

	while (1) {

		flag=1;
		while (flag) {
			MPI_Iprobe(MPI_ANY_SOURCE,t[np],MPI_COMM_WORLD,&flag,&status);
			//MPI_Probe(MPI_ANY_SOURCE,t[np],MPI_COMM_WORLD,&status);
			
			if (flag) {
				MPI::COMM_WORLD.Recv(t+status.MPI_SOURCE,1,MPI::INT,status.MPI_SOURCE,status.MPI_TAG);
				print_progress(t,T,np,NP,g,G,eps);
			}
		
		}

		sum=0;
		for (uint r=0;r<NP;r++)
			sum+=t[r];

		if (sum==T*NP) {
			break;
		}


		while (u01()*max_weight > *(last[sample=(uint)(u01()*(A-1))]->w))
			;
	
		retry_loan:
	
			;
	
		memcpy(loan->d,last[sample]->d,size_of_mem);
	
	
		loan->perturb();
	
		if (loan->prior_density()==0 || loan->perturb_density(last[sample]->param)==0)
			goto retry_loan;
	
		loan->simulate();
	
		if (( *(loan->d)=loan->distance()) <= eps ) {
	
			uint min=T;
			uint min_rank=NP; //find out which one is the slowest rank... this will just prioritize the lowest ones first since we have changed to 0,1 status!
			for (uint r=0;r<NP;r++)
				if (t[r]<min) {
					min=t[r];
					min_rank=r;
				}

			if (min_rank<NP)  //make sure it was assigned... 
				MPI::COMM_WORLD.Send(loan->d,size_of_mem,MPI::CHAR,min_rank,T*NP+g);
//				MPI::COMM_WORLD.Ibsend(loan->d,size_of_mem,MPI::CHAR,min_rank,T*NP+g);

			//best to print here, as only np==0 will be able to
			//print_progress(t,T,np,NP,g,G,eps);
		}
	}
*/

	//one last print...
	print_progress(t,T,np,NP,g,G,eps);

	MPI_Barrier(MPI_COMM_WORLD);
/*
	while (1) {
		MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
		if (flag) 
			MPI::COMM_WORLD.Recv(NULL,NULL,MPI::CHAR,status.MPI_SOURCE,status.MPI_TAG);
		else
			break;
	}
*/
	delete t;
}
