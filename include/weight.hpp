#define DELIM   "\t ," 
#define MAX(a,b)	((a)>(b) ? (a):(b)) 
#define MIN(a,b)	((a)<(b) ? (a):(b)) 

#define AL3C 1
#include "al3c.hpp"

float calc_max_weight(framework_t<param_t> **accepted, uint A_per_proc, uint np );

float calc_sum_weight(framework_t<param_t> **current, uint A_per_proc, uint np);

bool sortMethod(framework_t<param_t> *p1,framework_t<param_t> *p2);
