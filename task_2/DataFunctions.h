//
// Created by katerina on 17/1/2017.
//

#ifndef EXTERNALSORT_DATAFUNCTIONS_H
#define EXTERNALSORT_DATAFUNCTIONS_H

#include "ES_info.h"
#include "Record.h"

#define ES_SLOTS_FOR_RECORDS (BLOCK_SIZE-sizeof(DataBlock_info)) / sizeof (Record)


extern int search_blocks;

typedef struct {
    int entriesNum;
} DataBlock_info;


// Return BlockFile info

ES_info* ES_ReturnInfo(int fileDesc);


int ES_OpenFile( const char *fileName /* όνομα αρχείου */ );


//Create and initialize new Data Block

int ES_NewDataBlock(int fileDesc, int blockNumber);


//Copy Data Block info to block info pointer

int ES_GetDataBlockInfo(int fileDesc, DataBlock_info *block_info, int blockNumber);

//Update Data Block info with block_info

int ES_UpdateDataBlockInfo(int fileDesc, DataBlock_info block_info, int blockNumber);

// Insert record to data block

int ES_InsertToDataBlock(ES_info info, int blockNumber, Record record);

// Insert record to Heap File

int ES_InsertEntry(int fileDesc, Record record);

// Sort Block with Bubble Sort

int ES_BlockBubbleSort(int fileDesc, int blocknum, int fieldno);

// Merge 2 sorted blocks

int ES_MergeSort(char* file1,char* file2, int fieldno, int perasma, int id);

// Print block of Records

void ES_PrintBlockOfRecords(int fileDesc, int blocknum);

void ES_PrintFile(int fileDesc);

int ES_NewFile(char* FileName, void* block);

int ES_PairFiles(char* template, int num, int fieldNo, int perasma);

int ES_FinalizeHeap(char* filename, int fieldno);

int ES_CheckRecord(Record rec,int field, void* value);

int ES_searchblock(int filedesc,int fieldNo,void* value,int blocknum);

int ES_splitfile(int fileDesc,int fieldNo, void* value,int upper,int lower);


#endif //EXTERNALSORT_DATAFUNCTIONS_H
