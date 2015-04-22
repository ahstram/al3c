#mask=`taskset -p \`echo $$\` | cut -f2 -d':'`

macs $@ 2>&1  | allele_spectrum | cut -f3- | sed 1d

#taskset $mask macs $@ 2>&1 | taskset $mask allele_spectrum | taskset $mask cut -f3- | taskset $mask sed 1d 
#taskset -p $$ macs $@ 2>&1  | taskset -p $$ allele_spectrum | taskset -p $$ cut -f3- | sed 1d   #taskset -p $$ sed 1d #| taskset -p $$ cut -f3-
##1.244 --> [0.72, 1.56]
##3.12  --> [0.16, 0.48]
##1     --> [0.84, 1.24]
