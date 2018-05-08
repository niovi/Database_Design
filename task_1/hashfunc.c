
#include "hashfunc.h"





int Int_Hash_Function(int key, char* value){
    int rem, i;
    //32 bit Mix Functions for integer
    key = ~key + (key << 15); // key = (key << 15) - key - 1;
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057; // key = (key + (key << 3)) + (key << 11);
    key = key ^ (key >> 16);
    key = key%128;

    //key values 0-127 sto dekadiko,ara mexri 7bits sto diadiko.metatropi se diadiko:

    for (i=6; i>-1; i--){
        rem=key%2;
        if (rem==0)
            value[i]='0';
        else
            value[i]='1';
        key/=2;
    }
    return key;
}

int Char_Hash_Function(char* str, char* value){
    int hash = 0;
    int c, rem, i;

    c = *str++;
    while (c){
        hash = c + (hash << 6) + (hash << 16) - hash;
        c = *str++;
    }
    hash = hash%128;

    for (i=6; i>-1; i--){
        rem= (hash % 2);
        if (rem==0)
            value[i]='0';
        else
            value[i]='1';
        hash/=2;
    }
    return hash;
}



int HT_HashFunction(char *word,  int Bsize)
{
    unsigned int hash = 0;
    int i;
    for ( i = 0 ; word[i] != '\0' ; i++)
    {
        hash = 31*hash + word[i];
    }
    return hash % Bsize;
}