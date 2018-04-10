#include <iostream>

#include "../include/externs_typedefs.hpp"

void signal_callback_handler(int signum) {

    SIGNUM=signum;

    if (np==0)
        std::cerr<<"Received signal '"<<SIGNUM<<"', stand by while we finish this generation..."<<std::endl;

}
