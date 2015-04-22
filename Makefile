include make.inc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CPPFLAGS += -undefined dynamic_lookup
endif


all: lib/libmacs.so bin/al3c

bin/al3c:  src/main.cpp src/generate.cpp src/progress.cpp  src/signal.cpp  src/u01.cpp  src/weight.cpp  include/al3c.hpp
	$(MPI) $(MPIFLAGS) $(MPIINCLUDEFLAGS)  -o bin/al3c  $(MPILIBFLAGS) $<

lib/libmacs.so:	macs/libmacs.cpp include/al3c.hpp
	$(CPP) $(CPPFLAGS) $(CPPINCLUDEFLAGS)  -o lib/libmacs.so -fPIC $(CPPLIBFLAGS) $<

clean: 
	rm bin/al3c lib/libmacs.so


