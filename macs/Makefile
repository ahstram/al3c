include make.inc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
        CPPFLAGS += -undefined dynamic_lookup -stdlib=libstdc++ -lstdc++ 
endif

all: macs.so allele_spectrum

macs.so:    macs.so.cpp ../include/al3c.hpp
	$(CPP) -shared $(CPPFLAGS) $(CPPINCLUDEFLAGS)  -o macs.so -fPIC $(CPPLIBFLAGS) $<

allele_spectrum: allele_spectrum.cpp
	$(CPP) $(CPPFLAGS) $(CPPINCLUDEFLAGS)  -o allele_spectrum $<
clean:
	rm allele_spectrum macs.so
