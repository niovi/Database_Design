

#ifndef HASHTABLES_HASHFUNC_H
#define HASHTABLES_HASHFUNC_H
#include <stdio.h>
#include <math.h>

int Int_Hash_Function(int key, char* value);
int Char_Hash_Function(char* str, char* value);
int HT_HashFunction(char *word, int Bsize);


#endif //HASHTABLES_HASHFUNC_H
