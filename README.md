# al3c

Compilation should be a matter of running make in this directory. You may need to tweak some locations in make.inc if you run into any errors.

In this directory, you will find an example of a working implementation of distributed ABC using the coalescent simulator MaCS as the simulation engine. Please visit https://github.com/gchen98/macs to obtain the latest version, and add your MaCS installation directory to your search path.  Be sure macs can execute from this directory by typing 'macs'. To run the example, type:

bin/al3c cfg/macs-1x1.66.xml

If you have more than one processor, you may alternatively run al3c with command:

mpirun -np NP bin/al3c cfg/macs-1x1.66.xml

where you replace NP with the number of processors available.

For more info on al3c, please see our project page at http://ahstram.github.io/al3c/
