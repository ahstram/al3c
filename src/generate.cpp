void generate(float **O, uint N, uint D,framework_t<param_t> **proposed, uint T, framework_t<param_t> **last, framework_t<param_t> *loan, float max_weight, uint A_per_proc, uint A, float eps, uint np, uint NP, uint g, uint G, param_summary_t *summary,bool cheat, uint size_of_mem) {

	uint sample;

	uint *t=new uint[NP]();

	if(cheat && g>1)
		for (uint i=0;i<A_per_proc && t[np]<T;i++) {
			sample=np*A_per_proc+i;
			if (*(last[sample]->d)<=eps) {
				memcpy(proposed[t[np]]->d,last[sample]->d,size_of_mem);
				t[np]++;
			}
		}

	print_progress(t,T,np,NP,g,G,eps);

	
	for (;t[np]<T;t[np]++) {

		repeat: 

		
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


	}

	//one last print...
	print_progress(t,T,np,NP,g,G,eps);

	MPI_Barrier(MPI_COMM_WORLD);

	delete [] t;
}
