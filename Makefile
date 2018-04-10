include make.inc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
        MPIFLAGS += -stdlib=libstdc++ -lstdc++
endif

all: bin/al3c 

static_darwin: bin/al3c_darwin_x86-64

static_linux: bin/al3c_linux_x86-64

bin/al3c: .build/u01.o .build/SFMT.o .build/signal.o \
	.build/main.o .build/weight.o .build/mpi_check.o make.inc
	$(MPI) -o $@ .build/*.o -ldl -rdynamic

.build/main.o:  src/main.cpp include/al3c.hpp include/externs_typedefs.hpp \
	include/mpi_check.hpp include/signal.hpp include/SMC.hpp include/u01.hpp\
	include/externs_typedefs.hpp make.inc
	$(MPI) -c -o $@ $(MPIFLAGS) $(MPIINCLUDEFLAGS) $(MPILIBFLAGS) $<

.build/mpi_check.o: src/mpi_check.cpp include/al3c.hpp \
   	include/externs_typedefs.hpp make.inc
	$(MPI) -c -o $@ $(MPIFLAGS) $(MPIINCLUDEFLAGS) $(MPILIBFLAGS) $<

.build/signal.o: src/signal.cpp include/externs_typedefs.hpp
	g++ -c -o $@ $<

.build/SFMT.o: src/SFMT/SFMT.c
	g++ -c -o $@ $<

.build/u01.o: src/u01.cpp include/u01.hpp include/externs_typedefs.hpp
	g++ -c -o $@ $<

.build/weight.o: src/weight.cpp include/weight.hpp include/al3c.hpp make.inc
	$(MPI) -c -o $@ $(MPIFLAGS) $(MPIINCLUDEFLAGS) $(MPILIBFLAGS) $<

##make sure MPI .dylibs are removed for this to compile statically...
bin/al3c_darwin_x86-64: .build/u01.o .build/SFMT.o \
	.build/signal.o .build/main.o .build/weight.o .build/mpi_check.o make.inc
	$(MPI) $(MPIFLAGS) $(MPIINCLUDEFLAGS)  -o bin/al3c_darwin_x86-64  \
		$(MPILIBFLAGS) $<

bin/al3c_linux_x86-64: .build/u01.o .build/SFMT.o .build/signal.o \
	.build/main.o .build/weight.o .build/mpi_check.o make.inc
	mpiicpc -Wall -O2 -static_mpi -o bin/al3c_linux_x86-64 -ldl  $<

clean: 
	-rm bin/al3c bin/al3c_linux_x86-64 bin/al3c_darwin_x86-64 .build/*
