# al3c

We provide an al3c example for parallelized ABC-SMC using the coalescent simulator MaCS as the simulation engine.

Running the provided example with al3c on a Linux 64-bit system is as simple as issuing the following commands:

```bash
git clone https://github.com/ahstram/al3c.git  
cd al3c/  
bin/al3c_linux_x86-64 cfg/macs_linux-x86-64.xml  
```

To compile the example from source, you will need to ensure that an MPI library such as Intel MPI Library, OpenMPI or MPICH2 is installed, then follow:

```bash
git clone https://github.com/ahstram/al3c.git  
cd al3c/  
make  
## The following command compiles the MaCS binary, which requires C++ Boost libraries be installed
git clone https://github.com/gchen98/macs.git macs-binary && cd macs-binary && make && ln -s ../macs-binary/macs ../macs/macs && cd .. 
cd macs/ && make && cd ../  
bin/al3c cfg/macs.xml  
```

Modifications to the make.inc files in the al3c/ and al3c/macs/ directory may be necessary for your environment.
