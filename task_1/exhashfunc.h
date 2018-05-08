

#ifndef HASHTABLES_EXHASHFUNC_H
#define HASHTABLES_EXHASHFUNC_H

#include <math.h>
#include <stdio.h>

unsigned fnv_hash(void *key, int len);

void to_binary(unsigned int num, char *binary);

void to_decimal(char* binary, int* decimal, int buckets);

char *decimal_to_binary(int n);


#endif //HASHTABLES_EXHASHFUNC_H
