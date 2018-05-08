
#ifndef HASHTABLES_EXDATAFUNCTIONS_H
#define HASHTABLES_EXDATAFUNCTIONS_H

#include "EH_info.h"
#include "Record.h"

#define EH_SLOTS_FOR_RECORDS (BLOCK_SIZE-sizeof(DataBlock_info)) / sizeof (Record)
#define EH_SLOTS_FOR_BUCKETS (int)((BLOCK_SIZE-(sizeof (AddrTable_info)))/(sizeof(int)))

typedef struct {
    int localDepth;
    int entriesNum;
} DataBlock_info;

typedef struct {
    int overflow;
} AddrTable_info;


int EH_CreateNewBucket(EH_info *info, int decimal_hash);

int EH_DoubleHashTable(EH_info* header_info);

int EH_NewAddressTable(EH_info* header_info);

int EH_NewDataBlock(int fileDesc, int localDepth);

int EH_GetDataBlockInfo(int fileDesc, DataBlock_info *block_info, int blockNumber) ;

int EH_InsertToDataBlock(int fileDesc, int blockNumber, Record record) ;

int EH_GetAddrTable(EH_info* header, int *addressTable) ;

int EH_UpdateAddrTable(EH_info header, int *addressTable) ;

void EH_PrintBlockOfInts(void *block);

void EH_PrintBlockOfRecords(void *block);

#endif //HASHTABLES_EXDATAFUNCTIONS_H
