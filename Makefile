include make.inc

all: bin/al3c

bin/al3c:  src/main.cpp src/generate.cpp src/progress.cpp  src/signal.cpp  src/u01.cpp  src/weight.cpp  include/al3c.hpp
	$(MPI) $(MPIFLAGS) $(MPIINCLUDEFLAGS)  -o bin/al3c  $(MPILIBFLAGS) $<

clean: 
	rm bin/al3c
