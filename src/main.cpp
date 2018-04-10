#define LINE_LEN    1024
#define __STDC_LIMIT_MACROS

#include <iostream>
#include <sstream>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include <signal.h>

#include "rapidxml/rapidxml.hpp"

#define AL3C 1
#include "../include/al3c.hpp"
#include "../include/externs_typedefs.hpp"
#include "../include/mpi_check.hpp"
#include "../include/signal.hpp"
#include "../include/SMC.hpp"
#include "../include/u01.hpp"

uint np=0, NP=0, SIGNUM=0;

int main (int argc, char *argv[] ) {

    //configuration file...
    if (argc!=2) {
        std::cerr<<"Error! Run with "<<argv[0]<<" <XML configuration>"\
            <<std::endl;
        exit(EXIT_FAILURE);
    }

    std::ifstream xmlfile (argv[1]);

    if (!xmlfile) {
        std::cerr<<"Error! Could not open XML file '"<<argv[1]<<"'"<<std::endl;
        exit(EXIT_FAILURE);
    }

    //register signal handler
    signal(SIGINT, signal_callback_handler);

    //get MPI running...
    MPI::Init();

    np=MPI::COMM_WORLD.Get_rank(),  NP=MPI::COMM_WORLD.Get_size();

    std::vector<char> buffer((std::istreambuf_iterator<char>(xmlfile)), \
            std::istreambuf_iterator<char>());
    buffer.push_back('\0');
    rapidxml::xml_document<> config;    // character type defaults to char
    config.parse<0>(&buffer[0]);    // 0 means default parse flags


    if (!config.first_node("MPI")->first_node("NP")) {
        std::cerr<<"Error! Could not find required <MPI><NP></NP></MPI> in"\
            " XML file"<<std::endl;
        exit(EXIT_FAILURE);
    }

    //if not already running in MPI, this will invoke it for us...
    if (atoi(config.first_node("MPI")->first_node("NP")->value())!=(int)NP && \
            NP==1) {

        int returncode;

        char **args=new char*[4+argc];
        args[0]=strdup("mpirun");
        args[1]=strdup("-np");
        args[2]=strdup(config.first_node("MPI")->first_node("NP")->value());

        for (int i=0;i<argc;i++) {
            args[3+i]=argv[i];
        } args[3+argc]=NULL;

        if (check_mpirun())
            returncode=execvp("mpirun",args);
        else
            returncode=execvp("bin/mpirun",args);

        if (returncode!=0)
            std::cerr<<"Warning: mpirun exited with code '"<<returncode<<\
                "'"<<std::endl;

        for (int i=0;i<3;i++)
            free(args[i]);
        delete [] args;

        exit(returncode);
    }

    print_cpu_info();
u01();
    //initialize our ABC routine
    SMC_t SMC(&config);

    //begin the loop
    SMC.loop();

    delete [] rnd_array;
    // gracefully quit
    MPI::Finalize();

    exit(EXIT_SUCCESS);

}
