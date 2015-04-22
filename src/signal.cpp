#include <signal.h>

int SIGNUM=0;

void signal_callback_handler(int signum) {

	SIGNUM=signum;

	cerr<<"Received signal '"<<SIGNUM<<"', stand by while we finish this generation..."<<endl;

	exit(EXIT_FAILURE);
}
