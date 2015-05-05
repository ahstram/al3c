<pre>
         _______     
  ____ _/ /__  /_____  a software framework for numerical approximations
 / __ `/ / /_ >/ ___/  of posterior distributions by Approximate
/ /_/ / /___/ / /__    Bayesian Computation Sequential Monte Carlo
\__,_/_//____/\___/    in parallel 
</pre>

<i>al3c</i> is written for two types of users:

1) Those who have written their own software to address niche scientific questions, and would like to speed things up by using ABC in parallel without too much parallel programming or effort.  

2) Those seeking use ABC with coalescent theory models.   


To accomodate the latter users, we provide a number of examples using 
<i>al3c</i> with <i>MaCS</i>, a  Markovian coalescent theory simulator, which can
easily be modified for the user's specific case. 
</pre>

## al3c+MaCS Example 
We provide an al3c example for parallelized ABC-SMC using the coalescent simulator MaCS, available at https://github.com/gchen98/macs

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
## The following command compiles the MaCS binary, which requires C++ Boost libraries 
git clone https://github.com/gchen98/macs.git macs-binary && cd macs-binary && make && ln -s ../macs-binary/macs ../macs/macs && cd .. 
cd macs/ && make && cd ../  
bin/al3c cfg/macs.xml  
```

Modifications to the make.inc files in the al3c/ and al3c/macs/ directory may be necessary for your environment.
##Tutorial 

al3c consists of three components: the al3c binary, a user-defined shared library (.so file), and an XML file, which specifies parameters to be used with the user-defined shared library (.so file).
<pre>

 ___________
|           |
| XML file  |                      ________________        
|___________|                     |                |
             \  _________         |                |
              \|         |        |                |
               |   al3c  | -----> | desired output |
              /|_________|        |                |
 ___________ /                    |                |
|           |                     |________________|
| .so file  |
|___________|

</pre>
No modification is necessary to the al3c binary-- it is readily available in the git repository. 

We must create a shared library which will specify the:

- parameter space
- prior distribution
- parameter perturbation kernel
- simulation model
- distance function to compare observed & simulated data

and an XML configuration file, which will specify

- the location of the shared library (.so file)
- the observed datas
- number of processors to use
- basic ABC-SMC parameters

###Compiling the al3c binary

Note that compiling al3c requires MPI libraries be installed. To avoid installing MPI libraries on a Linux 64-bit system, you may instead use the static binary "bin/al3c_linux_x86-64" and skip this step.

The following commands will obtain the al3c source code and and compile the al3c binary

```bash
git clone http://github.com/ahstram/al3c.git
cd al3c
make
```

###Coding the shared library
Shared libraries for use with al3c should be written in C++, and include the "al3c.hpp" file which is found in the "include" directory. 

al3c.hpp defines two functions: 

<a href="#u01">u01</a>, which must be used as the shared library's random number generator, or used to seed any external random number generator, in order to ensure that different processors do not use the same random number seed. 

<a href="#exec_cmd">exec_cmd</a>, an optional function call external programs to simulate data

al3c.hpp declares two templates:

user_t, the users' parameters of interest

user_summary_t, a summary statistic for user_t

The user must then define all functions listed in <a href="#cpp_req">C/C++ Requirements</a>, in order to complete the definition of these two templates.

For a sample shared library's source code, please see this <a href="https://github.com/ahstram/al3c/blob/master/macs/macs.so.cpp">sample</a>.

###Compiling the shared library

###XML configuration

The XML configuration must define all nodes listed in <a href="#xmlreq">XML Requirements</a>. 

A sample XML file is available <a href="https://github.com/ahstram/al3c/blob/master/cfg/macs.xml">here</a>.

###Running al3c
al3c is run with the XML files to load as its sole argument, for example:

bin/al3c cfg/macs.xml

##Reference Manual</h2>
<pre>
1 C/C++ Provisions
        1.1 Variables
                1.1.1 <a href="#param">param</a> 
                1.1.2 <a href="#param_summary">param_summary</a>
                1.1.3 <a href="#S">S</a>
                1.1.4 <a href="#O">O</a>
                1.1.5 <a href="#N">N</a>
                1.1.6 <a href="#D">D</a>
                1.1.7 <a href="#d">d</a>
                1.1.8 <a href="#w">w</a>
        1.2 Functions
                1.2.1 <a href="#u01">u01</a>
                1.2.2 <a href="#exec_cmd">exec_cmd</a>
2 <span id="cpp_req">C/C++ Requirements</span> (<a href="https://github.com/ahstram/al3c/blob/master/macs/macs.so.cpp">sample</a>)
        2.1 Parameter & Parameter Summary Statistic Data Types
                2.1.1 <a href="#param_t">param_t</a>
                2.1.2 <a href="#param_summary_t">param_summary_t</a>
        2.2 Printing Parameter Data Type & Summarizing Acceptances
                2.2.1 <a href="#print">user_t::print</a>
                2.2.2 <a href="#summarize">user_summary_t::summarize</a>
        2.3 Generating the Prior Distribution
                2.3.1 <a href="#prior">user_t::prior</a> 
                2.3.2 <a href="#prior_density">user_t::prior_density</a>
        2.4 Perturbing Parameters
                2.4.1 <a href="#perturb">user_t::perturb</a>
                2.4.2 <a href="#perturb_density">user_t::perturb_density</a>
        2.5 Simulating Data & Measuring Goodness of Fit
                2.5.1 <a href="#simulate">user_t::simulate</a>
                2.5.2 <a href="#distance">user_t::distance</a>
3 <span id="xmlreq">XML Requirements</span> (<a href="https://github.com/ahstram/al3c/blob/master/cfg/macs.xml">sample</a>)
        3.1 <a href="#lib">lib</a> - Shared library
        3.2 MPI
                3.2.1 <a href="#NP">NP</a> - Number of Processors
        3.3 ABC
                3.3.1 <a href="#xmlG">G</a> - Generations
                3.3.5 <a href="#xmlR">R</a> - Rank
                3.3.4 <a href="#xmlE">E</a> - Epsilon
                3.3.3 <a href="#xmlA">A</a> - Acceptances
                3.3.2 <a href="#xmlT">T</a> - Trials
        3.4 <a href="#xmlO">O</a> - Observed data
</pre>

###C/C++ Provisions

####<span id="u01">u01</span>

```c++
float u01();
```

<i>Uniform random number generator</i>

<b>Example</b>

```c++
seed=u01();
seed_rng(seed);

while (1) {
        x=rng();
        ...
}
```
####<span id="exec_cmd">exec_cmd</span>

```cpp
uint exec_cmd(const char *cmd);
```

<i>Run command via system call</i>

<b>Example</b>

See <a href="#simulate">simulate()</a>

###C/C++: Parameter & Parameter Summary Statistics

####<span id="param_t">param_t</span>

```cpp
struct param_t;
```

<i>A user defined struct with the parameters we are investigating</i>

<b>Example</b>

```c++
struct param_t {

        float MigrationRate_EurToAfr,
              MigrationRate_AsnToAfr,
              MigrationRate_AsnToEur,
              EffectivePopulationSize_Afr,
              GrowthRate_Eur,
              GrowthRate_Asn,
              PastEvent_EurToAfrMigration;
};
```

####<span id="print">print</span>


```c++
const char *framework_t::print();
```

<i>Format parameter for printing</i>

<b>Relevant Variables</b>

```c++
param_t *param // parameter to format 
float *d //  the distance of param's simulation
float *w // // the weight of param
bool header // printing header or not
```

<b>Return Value</b>

```c++
const char *d <return value>;
```

<b>Example</b>

```cpp
user_t::print(bool header) {

        ofstringstream output;

        if (header)
                output<<"#distance        weight        MigrationRate_EurToAfr        MigrationRate_AsnToAfr        MigrationRate_AsnToEur        EffectivePopulationSize_Afr        GrowthRate_Eur        GrowthRate_Asn        PastEvent_EurToAfrMigration"<<endl;
        else
                output&lt<*d&lt<*w&lt<param->MigrationRate_EurToAfr<<param->MigrationRate_AsnToAfr<<param->MigrationRate_AsnToEur<<param->EffectivePopulationSize_Afr<<param->GrowthRate_Eur<<param->GrowthRate_Asn<<param->PastEvent_EurToAfrMigration<<endl;
                
        return output.str().c_str();
}
````

####<span id="param_summary_t">param_summary_t</span>
```cpp
struct param_summary_t;
```
<i>A user defined struct giving summary statistics necessary for a dynamic perturbation kernel</i>


<b>Example</b>
```cpp
struct param_summary_t {

        float MigrationRate_EurToAfr_Variance,
              MigrationRate_AsnToAfr_Variance,
              MigrationRate_AsnToEur_Variance,
              EffectivePopulationSize_Afr_Variance,
              GrowthRate_Eur_Variance,
              GrowthRate_Asn_Variance,
              PastEvent_EurToAfrMigration_Variance;

};
```

####<span id="summarize">summarize</span>
<i>Write to param_summary_t based on an array of param_t's</i>

```cpp
framework_t::summarize(param_t **params,uint A);
```
<b>Relevant Variables</b>

```cpp
param_t **params; // parameter to format 
uint A; // number of parameters 
```

<b>Return Value</b>

```cpp
param_summary_t *param_summary; // Desired statistics of accepted parameters
```

<b>Example</b>
```cpp
user_summary_t::summarize(param_t **params, uint A) {

        float m1=0,m2=0;

        for (uint a=0;a<A;a++) {
                m1+=params[a]->MigrationRate_EurToAfr;
                m2+=params[a]->MigrationRate_EurToAfr*params[a]->MigrationRate_EurToAfr;
        }
        m1/=(float)A;
        m2/=(float)A;

        param_summary->MigrationRate_EurToAfr_Variance=m2-m1*m1;

/*
        ...
        
        (repeat for other parameters)

        ...
*/

        m1=0, m2=0;

        for (uint a=0;a<A;a++) {
                m1+=params[a]->PastEvent_EurToAfrMigration;
                m2+=params[a]->PastEvent_EurToAfrMigration*params[a]->MigrationRate_PastEvent_EurToAfrMigration;
        } 
        m1/=(float)A;
        m2/=(float)A;

        param_summary->PastEvent_EurToAfrMigration_Variance=m2-m1*m1;


}
```
###C/C++: Generating the Prior Distribution

####<span id="prior">prior</span>
<i>Generate random parameters from the prior distribution</i>

<table bgcolor="lightgray"><tr><td>
void framework_t::prior();
</td></tr></table>
<b>Return Variables</b>

```cpp
param_t *param; // Parameter to write to according to the desired prior distribution
```

<b>Example</b>
```cpp
void user_t::prior() {

        param->MigrationRate_EurToAfr=u01()*0.4+0.84; // Unif[0.84,1.24]
        param->MigrationRate_AsnToAfr=u01()*0.32+0.16; // Unif[0.16,0.48]
        param->MigrationRate_AsnToEur=u01()*0.84+0.72; // Unif[0.72,1.56]
        param->EffectivePopulationSize_Afr=u01()*0.2994+1.319; // Unif[1.319, 1.6184]
        param->GrowthRate_Eur=u01()*0.31+0.28; // Unif[0.28, 0.59]
        param->GrowthRate_Asn=u01()*0.45+0.30; // Unif[0.30, 0.75]
        param->PastEvent_EurToAfrMigration=u01()*2.8+4.8; // Unif[4.8,7.6]
}
```


####<span id="prior_density">prior_density<span>
<i>Give the probability density of a parameter generated by prior()</i>

```cpp
framework_t::prior_density();
```

<b>Relevant Variables</b>

```cpp
param_t *param; // A parameter for which we want the pdf of according to the prior distribution
```

<b>Return Variables</b>

```cpp
float <return value>; // The density of param according to the prior distribution
```

<b>Example</b>

```cpp
void user_t::prior_density {

        if (0.84<=param->MigrationRate_EurToAfr && param->MigrationRate_EurToAfr<=1.24
                 && 0.16<=param->MigrationRate_AsnToAfr && param->MigrationRate_AsnToAfr<=0.48
                 && 0.72<=param->MigrationRate_AsnToEur && param->MigrationRate_AsnToEur<=1.56
                 && 1.319<=param->EffectivePopulationSize_Afr && param->EffectivePopulationSize_Afr<=1.6184
                && 0.28<=param->GrowthRate_Eur && param->GrowthRate_Eur<=0.59
                 && 0.3<=param->GrowthRate_Asn && param->GrowthRate_Asn<=0.75
                && 4.8<=param->PastEvent_EurToAfrMigration && param->PastEvent_EurToAfrMigration<=7.6)
                return 1;
        else
                return 0;
}

```

###C/C++: Perturbing Variables

####<span id="perturb">perturb</span>
```cpp
void framework_t::perturb();
```
<i>Perturb parameters according to a desired perturbation kernel</i>

<b>Relevant Variables</b>

```cpp
param_summary_t *param_summary; // A user defined struct giving summary statistics of the last generation's accepted parameters
param_t *param; // The parameter to be perturbed

<b>Return Variables</b>

param_t *param; // The perturbed parameter

<b>Example</b>

```cpp
void user_t::perturb() {

        param->MigrationRate_EurToAfr+=(u01()-0.5f)*sqrt(2*param_summary->MigrationRate_EurToAfr_Variance*12);

        /* ... repeat for other parameters ... */

        param->PastEvent_EurToAfrMigration+=(u01()-0.5f)*sqrt(2*param_summary->PastEvent_EurToAfrMigration*12);
}
```


####<span id="perturb_density">perturb_density</span>

```cpp
float framework_t::perturb_density(param_t *old_param);
```

<i>Give the probability density of a parameter perturbation</i>

<b>Relevant Variables</b>

<font color="green">param_t *old_param</font> - A parameter from last generation's acceptances
<font color="green">param_t *param</font> - The perturbed parameter
<font color="green">param_summary_t *summary</font> - A user defined struct giving summary statistics of the last generation's accepted parameters

<b>Return Variables</b>

<font color="red">float <return value></font> - The probability density of <font color="green">param</font> being perturbed from <font color="green">old_param</font>, according to the perturbation kernel specified in perturb()
 
<b>Example</b>

```cpp
float user_t::perturb_density() {

        if ( fabs(param->MigrationRate_EurToAfr - old_param->MigrationRate_EurToAfr) > sqrt(2*param_summary->MigrationRate_EurToAfr_Variance*12)/2.f ) 
                return 0.f;

        /* ... repeat for other parameters ... */

        if ( fabs(param->PastEvent_EurToAfrMigration - old_param->PastEvent_EurToAfrMigration) > sqrt(2*param_summary->PastEvent_EurToAfrMigration_Variance*12)/2.f ) 
                return 0.f;

        return 1.f;        
}
```

###C/C++: Data simulation

####<span id="simulate">simulate</span>
<i>Simulate data according to given parameters</i>

<table bgcolor="lightgray"><tr><td>
framework_t::simulate();
</td></tr></table>
<b>Revelant Variables</b>

<font color="green">param_t *param</font> - The parameter to simulate with
<font color="blue">uint N</font> -  The number of rows of simulated/observed data
<font color="blue">uint D</font> -  The number of columns (<b>D</b>imensions) of simulated/observed data

<b>Return Variables</b>

<font color="brown">float **S</font> -  An <font color="blue">N</font>*<font color="blue">D</font> array of floating points containing simulated data

<b>Example</b>

```cpp
user_t::simulate() {

        ostringstream cmd;

        uint seed=(uint)u01()*UINT_MAX;

        cmd<<"macs.sh 718 100000 -s "<<seed<<" -t .001 -I 3 176 170 372 0 -m 2 1 "<<param->MigrationRate_EurToAfr<<" -m 3 1 "<<param->MigrationRate_AsnToAfr<<" -m 3 2 "<<param->MigrationRate_AsnToEur<<" -n 1 "<<param->EffectivePopulationSize_Afr<<" -g 2 "<<param->GrowthRate_Eur<<" -g 3 "<<param->GrowthRate_Asn<<" -eg .0230000 2 0 -eg .0230001 3 0 -ej .0230002 3 2 -em .0230003 2 1 "<<param->PastEvent_EurToAfrMigration<<" -en .0230004 2 0.1861 -ej .051 2 1 -en .148 1 0.731 -r 0.0006"<<endl;

        //this is a helper function that will write to **S
        exec_cmd(cmd.str().c_str());
}
```

####<span id="distance">distance</span>

<i>Give the distance between simulated & observed data</i>

```cpp
float framework_t::distance();
```

<b>Relevant Variables</b>

<font color="blue">uint N</font> -  The number of rows of simulated/observed data
<font color="blue">uint D</font> -  The number of columns (<b>D</b>imensions) of simulated/observed data
<font color="green">float **S</font> - An <font color="blue">N</font>x<font color="blue">D</font> matrix of floating points containing simulated data
<font color="blue">float **O</font> - An <font color="blue">N</font>x<font color="blue">D</font> matrix of floating points containing observed data

<b>Return Variables</b>

<font color="red">float <return value></font> - The distance between observed & simulated data
 
<b>Example</b>
```cpp

user_t::distance() {

        float r=0;

        for (uint n=0;n<N;n++)
                for (uint d=0;d<D;d++)
                        r+=pow(S[n][d]-O[n][d],2);

        return sqrt(r);
}
```

###XML: lib

####<span id="lib">lib</span>

<i>C++ library to load</i>

<b>Notes</b>

<b>Example</b>
```xml
<lib>lib/libmacs.so</lib>
```



###XML: MPI

####<span id="NP">NP</span>
<i>Number of processors to use</i>

<b>Notes</b>
This should be the total number of processors <i>al3c</i> will be run with. If there are 8 nodes, each with 8 processors, we specify NP as "64" and let the MPI system do the rest.

<b>Example</b>
```xml
<MPI>
        <NP>64</NP>  
</MPI>
```


###XML: ABC


####<span id="xmlG">G</span>
<i>Minimum number of generations of ABC-SMC to run before al3c quits</i>

<b>Acceptable values</b>
integers in {0,1,...}

<b>Notes</b>
This is the minimum number of generations of ABC-SMC to run. 

<i>al3c</i> will not quit until both "G" and "<a href="#xmlE">E</a>" are satisfied. If you'd like to quit strictly based on "G", set "E" to be "0".

<b>Example</b>
```xml
<ABC>
        <G>15</G>  
</ABC>
```

####<span id="xmlR">R</span>
<i>Rank of accepted parameter to set next generation's epsilon to</i>

<b>Acceptable values</b>
integers in {1,2,...,A}
integers in {1,2,...,A}<sup>G</sup>

<b>Notes</b>
After each generation, the distances for each accepted parameter are sorted, least to greatest. If we would like to set the next generation's epsilon ("acceptance threshold") according to the present generation's accepted parameter distances, we do so here. This allows us to adaptively set our epsilon schedule according to quantiles, rather than having to set it explicitly beforehand.

If "A" is set to "1000", and we want to set our epsilon to the simulation with the 250th best rank, we set "250" here. All accepted simulations in the next generation will have a distance less than or equal to this generation's 250th best simulation.

If we would like to change the quantile used for each generation, we can specify R as a tab/space delimited vector.

0 means "ignore". al3c will fall back on an "E" vector to set its epsilon schedule. If "E" is a scalar, acceptances will be made according to "A" and "T" only.


<b>Example</b>
```xml
<ABC>
        <R>250</R>  
</ABC>

<!-- Or, if we'd like to vary the rank over generations, do it like this: -->

<ABC>
        <E>750 500 400 300 250 250 250 250 250 250 250 250 250 300 400</E>
</ABC>
```


####<span id="xmlE">E</span>
<i>Maximum distance to observed data all accepted simulations in a generation must have before al3c quits</i>

<b>Acceptable values</b>
reals in [0,&infin;)
strictly decreasing reals in [0,&infin;)<sup>G</sup>

<b>Notes</b>
When all accepted parameters in the most recent generation have simulated datasets with a distance from observed data less than or equal to "E", we quit.

If you desire to explicitly set a rejection threshold schedule  ("epsilon schedule"), you can do so by letting this value be a vector of G strictly decreasing values, delimited by spaces or tabs. In that case, "<a href="#xmlR">R</a>" must be set to "0". Otherwise, use "R" for dynamic rank-based epsilon schedules. 

<i>al3c</i> will not quit until both "<a href="#xmlG">G</a>" and "E" are satisfied. If you'd like to quit strictly based on "E", set "G" to be "0".

<b>Example</b>
```xml
<ABC>
        <E>1.4</E>  
</ABC>

<!-- Or, if we'd like to set an explicit epsilon schedule, do it like this: -->

<ABC>
        <E>100 90 80 70 60 50 40 30 20 10 5 4 3 2 1</E>  
</ABC>
```


####<span id="xmlA">A</span>
<i>Number of accepted trials ("acceptances") per generation</i>

<b>Acceptable values</b>
integers in {1,2,...,T}

<b>Notes</b>
This is the number of accepted parameters we desire per generation.

The actual value used may be slightly higher than specified here, so that "<a href="#NP">NP</a>" divides "A", allowing for even allocation of computation across processors.


<b>Example</b>
```xml
<ABC>
        <A>1000</A>  
</ABC>
```

####<span id="xmlT">T</span>
<i>Minimum number of trials per generation</i>

<b>Acceptable values</b>
integers in {1,2,...}

<b>Notes</b>
This is the minimum number of trials we must have per generation. Each trial is either accepted or rejected. There may be more trials than specified here depending on the values of "<a href="#xmlE">E</a>" and "<a href="#xmlR">R</a>" are set.

The actual value used may be slightly higher than specified here, so that "<a href="#NP">NP</a>" divides "T", allowing for even allocation of computation across processors.


<b>Example</b>
```xml
<ABC>
        <T>10000</T>  
</ABC>
```


###XML: O

####<span id="xmlO">O</span>
<i>Observed data to simulate</i>

<b>Notes</b>
This can be a 2-dimensional matrix, rows delimited by newlines, columns delimited by tabs/spaces/columns. The number of rows and columns will be counted, and the values of "N" and "D" will be inferred accordingly.

<b>Example</b>
```xml
<O>259        108        103        147
119        53        66        132
100        46        61        128</O>
```

