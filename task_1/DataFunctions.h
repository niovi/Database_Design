



#ifndef HASHTABLES_DATAFUNCTIONS_H
#define HASHTABLES_DATAFUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "BF.h"
#include "HT_info.h"
#include "hash.h"
#include "Record.h"

#define HT_SLOTS_FOR_BUCKETS (int)((BLOCK_SIZE-(sizeof (AddrTable_info)))/(sizeof(AddressEntry)))
#define HT_SLOTS_FOR_RECORDS (BLOCK_SIZE-sizeof(DataBlock_info)) / sizeof (Record)

typedef struct {
    int blocks;
    int records;
} Bucket_info;

typedef struct {
    int overflow;
    int entriesNum;
} DataBlock_info;

typedef struct {
    int overflow;
} AddrTable_info;

typedef struct {
    int first_block;
    int last_block;
} AddressEntry;


Bucket_info HT_GetBucketInfo(int fileDesc,int bucket_num);

int HT_NewAddrTableBlock(int fileDesc, int buckets);


//Create and initialize new Data Block

int HT_NewDataBlock(int fileDesc, int blockNumber);

//Copy Data Block info to block info pointer

int HT_GetDataBlockInfo(int fileDesc, DataBlock_info *block_info, int blockNumber);
//Update Data Block info with block_info

int HT_UpdateDataBlockInfo(int fileDesc, DataBlock_info block_info, int blockNumber);

//insert record to data block

int HT_InsertToDataBlock(HT_info header,int bucket, int blockNumber, Record record);

//copy the address table from blocks to addressTable pointer

int HT_GetAddrTable(int fileDesc, AddressEntry *addressTable);

// print address table

//void HT_PrintAddressTable(AddressEntry *addressTable, int buckets)

void HT_PrintBlockOfInts(void *block);


#endif //HASHTABLES_DATAFUNCTIONS_H
