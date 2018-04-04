#include <iostream>
using namespace std;
#include "externs.hpp"

void signal_callback_handler(int signum) {

	SIGNUM=signum;
	
	if (np==0)
		cerr<<"Received signal '"<<SIGNUM<<"', stand by while we finish this generation..."<<endl;

}
