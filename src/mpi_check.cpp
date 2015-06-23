int check_mpirun() {

 struct stat sb;
  string delimiter = ":";
  string path = string(getenv("PATH"));
  size_t start_pos = 0, end_pos = 0;

  while ((end_pos = path.find(':', start_pos)) != string::npos)
    {
      string current_path =
        path.substr(start_pos, end_pos - start_pos) + "/mpirun";

      if ((stat(current_path.c_str(), &sb) == 0) && (sb.st_mode & S_IXOTH))
        {
          return 1;
         }

      start_pos = end_pos + 1;
     }

  return 0;
}

void print_cpu_info()
 {

        int len,nprocs,rank;
    char name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(name, &len);


cerr<<"processor "<<rank+1<<"/"<<nprocs<<" on "<<name<<" initialized"<<endl;
}


