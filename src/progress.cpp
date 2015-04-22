
void print_progress(uint *t, uint T, uint np, uint NP, uint g, uint G, float eps) {

//	MPI_Barrier(MPI_COMM_WORLD);

	if (np==0) {
		cerr<<"\rgeneration="<<g<<"/"<<G<<", epsilon="<<eps<<", simulations=("<<t[0]<<"/"<<T;
		for (uint r=1;r<NP;r++) {
			cerr<<","<<t[r]<<"/"<<T;
		}
		cerr<<")...";
	}

}


void update_progress(uint *t,uint T, uint np, uint NP, float eps, MPI_Request *mpi_request) {

//	if (t[np]>0)
//		MPI_Wait(mpi_request+np, MPI_STATUS_IGNORE);


                //MPI_Iprobe(MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&flag,&status);
//ORIGINAL:
	//want the one that sent to wait...
//	for (uint r=0;r<NP;r++) {

//		MPI_Bcast(t+r,1,MPI::INT,r,MPI_COMM_WORLD);
//		MPI_Ibcast(t+r,1, MPI::INT,r,MPI_COMM_WORLD, mpi_request+r);
			
//	}
//MPI_Wait(mpi_request+np, MPI_STATUS_IGNORE);

/*

	for (uint r=0;r<NP;r++)
		if (r!=np)
			MPI::COMM_WORLD.Send(t+np,1,MPI::INT,r,t[np]);	
*/

	
/*
	//if (t[np]<T-1)
		for (uint r=0;r<NP;r++)
			if (r!=np) //send t[np] to every r!=np process
				MPI::COMM_WORLD.Send(t+np,1,MPI::INT,r,t[np]);


	int flag=1;
	MPI_Status status;

	while (flag) {
        	//MPI_Iprobe(MPI_ANY_SOURCE,t[np],MPI_COMM_WORLD,&flag,&status);

		MPI_Probe(MPI_ANY_SOURCE,t[np],MPI_COMM_WORLD,&status);

		if (flag)
                        MPI::COMM_WORLD.Recv(t+status.MPI_SOURCE,1,MPI::INT,status.MPI_SOURCE,status.MPI_TAG);
	}
*/
}


