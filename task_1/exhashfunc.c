#include <malloc.h>
#include "exhashfunc.h"

char *decimal_to_binary(int n) {

    int c, d, count;
    char *pointer;

    count = 0;
    pointer = (char*)malloc(7+1);


    for ( c = 6 ; c >= 0 ; c-- )
    {
        d = n >> c;

        if ( d & 1 )
            *(pointer+count) = 1 + '0';
        else
            *(pointer+count) = 0 + '0';

        count++;
    }
    *(pointer+count) = '\0';

    return  pointer;

}

unsigned fnv_hash(void *key, int len) {
    unsigned char *p = (unsigned char *) key;
    unsigned int h = 2166136261;
    int i;
    for (i = 0; i < len; i++) {
        h = (h * 16777619) ^ p[i];
    }

    return h;
}

void to_binary(unsigned int num, char binary[33]) {
    unsigned int quotient = num / 2;
    int remainder = num % 2;
    binary[32] = '\0';
    binary[31] = (char) ('0' + remainder);
    int count = 30;
    while (quotient >= 0 && count >= 0) {
        remainder = quotient % 2;
        quotient = quotient / 2;
        binary[count] = (char) ('0' + remainder);
        count--;

    }
    int i;
    for (i = 0; i < 32; i++) {
        if (binary[i] != '0' && binary[i] != '1')
            binary[i] = '0';
    }
    return;
}

void to_decimal(char* binary, int* decimal, int buckets) {
    *decimal = 0;
    int i;
    for (i = 0; i < buckets; i++) {
        if (binary[i] == '1')
            *decimal = *decimal + (buckets - (i + 1));
    }
}