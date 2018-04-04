#define RANDOM_SEED 1 //be careful: without this, you may run on repeat after a while!
#define RND_BUFFER  16777216
#define SKEW_SEEDS  256

#define LEFT_OPEN   1 // U(0,1]
#define RIGHT_OPEN  2 // U[0,1)
#define BOTH_OPEN   3 // U(0,1)

#include "externs_typedefs.hpp"

uint32_t *skew_seed(bool random); 

float u01(char c);

float u01();

extern uint np;
extern uint32_t *rnd_array;
