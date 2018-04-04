include make.inc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
        MPIFLAGS += -stdlib=libstdc++ -lstdc++
endif

all: bin/al3c 

static_darwin: bin/al3c_darwin_x86-64

static_linux: bin/al3c_linux_x86-64

bin/al3c: bin/SMC.o bin/u01.o bin/SFMT.o bin/signal.o bin/main.o
	$(MPI) -o bin/al3c bin/SMC.o bin/u01.o bin/SFMT.o bin/signal.o bin/main.o

bin/main.o:  src/main.cpp #include/al3c.hpp make.inc
	$(MPI) -c -o bin/main.o $(MPIFLAGS) $(MPIINCLUDEFLAGS) $(MPILIBFLAGS) $< -ldl

bin/SMC.o: src/SMC.cpp
	$(MPI) -c -o bin/SMC.o $(MPIFLAGS) $(MPIINCLUDEFLAGS) $(MPILIBFLAGS) $<

bin/u01.o: src/u01.cpp
	g++ -c -o bin/u01.o $<

bin/SFMT.o: src/SFMT/SFMT.c
	g++ -c -o bin/SFMT.o $<

bin/signal.o: src/signal.cpp
	g++ -c -o bin/signal.o $<

bin/weight.o: src/weight.cpp
	$(MPI) -c -o bin/weight.o $(MPIFLAGS) $(MPIINCLUDEFLAGS) $(MPILIBFLAGS) $<

##make sure MPI .dylibs are removed for this to compile statically...
bin/al3c_darwin_x86-64:  src/main.cpp src/SMC.cpp src/signal.cpp  src/u01.cpp  src/weight.cpp  include/al3c.hpp make.inc
	$(MPI) $(MPIFLAGS) $(MPIINCLUDEFLAGS)  -o bin/al3c_darwin_x86-64  $(MPILIBFLAGS) $<

bin/al3c_linux_x86-64: src/main.cpp src/SMC.cpp src/signal.cpp  src/u01.cpp  src/weight.cpp  include/al3c.hpp make.inc
	mpiicpc -Wall -O2 -static_mpi -o bin/al3c_linux_x86-64 -ldl  $<

clean: 
	-rm bin/al3c bin/al3c_linux_x86-64 bin/SFMT.o bin/al3c_pre.o bin/u01.o \
		bin/weight.o bin/signal.o bin/SMC.o bin/main.o
