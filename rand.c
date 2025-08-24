#include "types.h"
#include "defs.h"

static uint seed = 1;

// simple rand function
int rand(void)
{
    seed = seed * 1664525 + 1013904223;
    return (seed >> 16) & 0x7FFF; // RAND_MAX assumed to be 32767
}

// set the seed for rand
void srand(uint s)
{
    seed = s;
}
