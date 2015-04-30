include make.inc

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
        CPPFLAGS += -undefined dynamic_lookup -stdlib=libstdc++ -lstdc++ 
endif

all: macs.so allele_spectrum

linux64: macs_linux_x86-64.so allele_spectrum_linux_x86-64

macs.so:    macs.so.cpp ../include/al3c.hpp
	$(CPP) -shared $(CPPFLAGS) $(CPPINCLUDEFLAGS)  -o macs.so -fPIC $(CPPLIBFLAGS) $<

macs_linux_x86-64.so:    macs_linux_x86-64.so.cpp ../include/al3c.hpp
	$(CPP) -shared $(CPPFLAGS) $(CPPINCLUDEFLAGS)  -o macs.so -fPIC $(CPPLIBFLAGS) $<

allele_spectrum: allele_spectrum.cpp
	$(CPP) $(CPPFLAGS) $(CPPINCLUDEFLAGS)  -o allele_spectrum $<

allele_spectrum_linux_x86-64: allele_spectrum.cpp
	$(CPP) $(CPPFLAGS) $(CPPINCLUDEFLAGS) -static  -o allele_spectrum_linux_x86-64 $<

clean:
	rm allele_spectrum allele_spectrum_linux_x86-64 macs.so macs_linux_x86-64.so
