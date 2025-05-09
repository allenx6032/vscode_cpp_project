#ifndef DRAND48_H  
#define DRAND48_H  
#include <stdlib.h>  
#define d48_m 0x100000000LL  
#define d48_c 0xB16  
#define d48_a 0x5DEECE66DLL  
static unsigned long long seed = 1;  
double drand48(void)  
{  
    seed = (d48_a * seed + d48_c) & 0xFFFFFFFFFFFFLL;  
    unsigned int x = seed >> 16;  
    return  ((double)x / (double)d48_m);  
}  
void srand48(unsigned int i)  
{  
    seed  = (((long long int)i) << 16) | rand();  
}  
#endif