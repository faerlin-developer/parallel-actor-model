#include <cstdlib>
#include "util/random.h"

/**
 * Set random seed.
 */
void set_random_seed(int seed) {
    srand(seed);
}

/**
 * Generates a random integer between two values, including the from value up to the to value minus
 * one, i.e. from=0, to=100 will generate a random integer between 0 and 99 inclusive
 **/
int get_random_integer(int from, int to) {
    return (rand() % (to - from)) + from;
}