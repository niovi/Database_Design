//
// Created by katerina on 17/1/2017.
//

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include "DataFunctions.h"
#include "BF.h"
#include "Record.h"
#include "ES_info.h"
#include "Sorted.h"

// Return BlockFile info

ES_info* ES_ReturnInfo(int fileDesc){

    //printf("ES_ReturnInfo\n");


    ES_info* info = malloc(sizeof(ES_info));
    void* block = malloc(sizeof(BLOCK_SIZE));
    if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
        BF_PrintError(" HT_NewDataBlock: Cannot read new data block\n");
        return NULL;
    }
    memcpy(info, block, sizeof (ES_info));
    //info = block;
    //printES_info(info);
    //free(info);
    //free(block);
    return info;
}

//Create and initialize new Data Block

int ES_NewDataBlock(int fileDesc, int blockNumber) {

    //printf("ES_NewDataBlock\n");


    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError(" ES_NewDataBlock: Cannot allocate new data block\n");
        return -1;
    }

    void* block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError(" ES_NewDataBlock: Cannot read new data block\n");
        return -1;
    }

    DataBlock_info block_info;
    block_info.entriesNum = 0;
    memcpy(block, &block_info, sizeof (DataBlock_info));

    if (BF_WriteBlock(fileDesc, blockNumber) < 0) {
        BF_PrintError(" ES_NewDataBlock: Cannot write to new data block\n");
        return -1;
    }
    //printf("ES_NewDataBlock: Success!\n");
    return 0;
}

//Copy Data Block info to block info pointer

int ES_GetDataBlockInfo(int fileDesc, DataBlock_info *block_info, int blockNumber) {
    void *block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        printf("Could not read block no: %d",blockNumber);
        BF_PrintError(" ES_GetDataBlockInfo: Cannot read data block\n");
        return -1;
    }
    memcpy(block_info,block, sizeof(DataBlock_info));
    //printf("HT_GetDataBlockInfo: Success!\n");
    return 0;
}

//Update Data Block info with block_info
int ES_OpenFile( const char *fileName /* όνομα αρχείου */ ){

    //printf("Sorted_OpenFile\n");


    int fileDesc;
    void* block = malloc(sizeof(BLOCK_SIZE));
    ES_info* info = malloc(sizeof(ES_info));

    if( (fileDesc=BF_OpenFile(fileName))<0 ){
        BF_PrintError("Could not open file");
        return -1;
    }
    if( BF_ReadBlock(fileDesc,0,&block)<0 ){
        BF_PrintError("Could not read block");
        return -1;
    }
    memcpy(info,block,sizeof(ES_info));
    info->fileDesc = fileDesc;
    memcpy(block,info,sizeof(ES_info));
    if( BF_WriteBlock(fileDesc,0)<0 ){
        BF_PrintError("Could not write block");
        return -1;
    }



    // 0 -> not sorted
    // 1 -> sorted
    if(info->type==1){
        //printf("Sorted File.\n");
        info->fileDesc = fileDesc;
        memcpy(block,info,sizeof(ES_info));
        if( BF_WriteBlock(fileDesc,0)<0 ){
            BF_PrintError("Could not write block");
            return -1;
        }
        return fileDesc;
    }else{
        //printf("Not Sorted File.\n");
        info->fileDesc = fileDesc;
        memcpy(block,info,sizeof(ES_info));
        if( BF_WriteBlock(fileDesc,0)<0 ){
            BF_PrintError("Could not write block");
            return -1;
        }
        return fileDesc;
    }

}
int ES_UpdateDataBlockInfo(int fileDesc, DataBlock_info block_info, int blockNumber) {
    void* block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError(" ES_UpdateDataBlockInfo: Cannot read data block\n");
        return -1;
    }
    memcpy(block, &block_info, sizeof (DataBlock_info));
    if (BF_WriteBlock(fileDesc, blockNumber) < 0) {
        BF_PrintError(" ES_UpdateDataBlockInfo:Cannot write to data block\n");
        return -1;
    }
    //printf("HT_UpdateDataBlockInfo: Success!\n");
    return 0;
}

// Insert record to data block

int ES_InsertToDataBlock(ES_info info, int blockNumber, Record record) {
    void* block = malloc(sizeof(BLOCK_SIZE));

    //printf("ES_InsertToDataBlock\n");


    DataBlock_info block_info;
    if (ES_GetDataBlockInfo(info.fileDesc, &block_info, blockNumber) < 0) {
        BF_PrintError(" ES_InsertToDataBlock: Cannot get data block info\n");
        return -1;
    }


    if (block_info.entriesNum < ES_SLOTS_FOR_RECORDS ) {
        //if there is still space in the block
        //copy record to block
        if (BF_ReadBlock(info.fileDesc, blockNumber, &block) < 0) {
            BF_PrintError(" ES_InsertToDataBlock: Cannot read data block\n");
            return -1;
        }
        memcpy(block + sizeof (DataBlock_info)+(block_info.entriesNum * sizeof (Record)), &record, sizeof (Record));
        block_info.entriesNum = block_info.entriesNum + 1;
        memcpy(block, &block_info, sizeof (DataBlock_info));
        if (BF_WriteBlock(info.fileDesc, blockNumber) < 0) {
            BF_PrintError(" ES_InsertToDataBlock: Cannot write to data block\n");
            return -1;
        }
        //printf("Record put in block no %d\n",blockNumber);
        //printRecord(record);
        //printf("ES_InsertToDataBlock: Success!\n");
        return 0;
    }



    else if (block_info.entriesNum == ES_SLOTS_FOR_RECORDS) {
        // if there is no space left in block and there is no overflow block
        //create a new block
        //printf("No overflow block!\n");


        int totalBlockNum;
        if ((totalBlockNum = BF_GetBlockCounter(info.fileDesc)) == -1) {
            BF_PrintError(" HT_InsertToDataBlock: BF_GetBlockCounter error\n");
            return -1;
        }


        if (ES_NewDataBlock(info.fileDesc, totalBlockNum) < 0) {
            BF_PrintError("HT_NewDataBlock");
            return -1;
        }

        return ES_InsertToDataBlock(info, totalBlockNum, record);
    }




    else{
        //printf("overflow: %d  num of records: %d\n",block_info.overflow,block_info.entriesNum);
        //printf("HT_InsertToDataBlock: reached end -> ERROR\n");
        return -1;}
}



// Sort Block with Bubble Sort

int ES_BlockBubbleSort(int fileDesc, int blocknum, int fieldno){

    //creating name for new file
    char name[20]="sorted-block-";
    char str[10];
    snprintf(str, 10, "%d", blocknum);
    strcpy(name,strcat(name, str));
    //printf("name is: %s\n",name);

    //variable to use
    //printf("BubbleSorting by field no %d\n",fieldno);
    void* block = malloc(sizeof(BLOCK_SIZE));
    Record record1,record2;
    DataBlock_info b1;
    int swapped=5;
    int rep=0;


    // read block and get info
    if (BF_ReadBlock(fileDesc, blocknum, &block) < 0) {
        BF_PrintError(" ES_BlockBubbleSort: Cannot read data block\n");
        return -1;
    }
    memcpy(&b1,block,sizeof(DataBlock_info));


    //begin sorting
    while(swapped>0){
        rep++;
        //printf("Bubble Sort Repetition no %d\n",rep);
        swapped=0;
        int l=0;

        while(l<b1.entriesNum-1){

            memcpy(&record1, (block + sizeof(DataBlock_info) + l * sizeof(Record)), sizeof(Record));
            memcpy(&record2, (block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)), sizeof(Record));

            //printRecord(record1);
            //printRecord(record2);
            //printf("id1: %d is less than id2: %d -> %d\n",record1.id,record2.id,record1.id>record2.id);


            if(fieldno==0){
                if(record1.id>record2.id){
                    memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                    memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                    l++;
                    swapped++;
                    //printf("swapping %d\n", swapped);
                }else{
                    l++;
                    //printf("leave as is\n");
                }
            }else if(fieldno==1){
                if(strcmp(record1.name,record2.name)==0){
                    //printf("Same name\n");
                    if(record1.id>record2.id){
                        memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                        memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                        l++;
                        swapped++;
                        //printf("swapping %d\n", swapped);
                    }else{
                        l++;
                        //printf("leave as is\n");
                    }
                }else if(strcmp(record1.name,record2.name)>0){
                    memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                    memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                    l++;
                    swapped++;
                    //printf("swapping %d\n", swapped);
                }else{
                    l++;
                    //printf("leave as is\n");
                }
            }else if(fieldno==2){
                if(strcmp(record1.surname,record2.surname)==0){
                    if(record1.id>record2.id){
                        memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                        memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                        l++;
                        swapped++;
                        //printf("swapping %d\n", swapped);
                    }else{
                        l++;
                        //printf("leave as is\n");
                    }
                }else if(strcmp(record1.surname,record2.surname)>0){
                    memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                    memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                    l++;
                    swapped++;
                    //printf("swapping %d\n", swapped);

                } else{
                    l++;
                    //printf("leave as is\n");
                }
            }else if(fieldno==3){
                if(strcmp(record1.city,record2.city)==0){
                    if(record1.id>record2.id){
                        memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                        memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                        l++;
                        swapped++;
                        //printf("swapping %d\n", swapped);
                    }else{
                        l++;
                        //printf("leave as is\n");
                    }
                }else if(strcmp(record1.city,record2.city)>0){
                    memcpy((block + sizeof(DataBlock_info) + l * sizeof(Record)),&record2, sizeof(Record));
                    memcpy((block + sizeof(DataBlock_info) + (l+1) * sizeof(Record)),&record1, sizeof(Record));
                    l++;
                    swapped++;
                    //printf("swapping %d\n", swapped);

                } else{
                    l++;
                    //printf("leave as is\n");
                }
            }else{
                printf("Wrong number\n");
                return -1;
            }


        }


    }

    // block is now sorted

    // write back to original file
    if (BF_WriteBlock(fileDesc, blocknum) < 0) {
        BF_PrintError(" ES_BlockBubbleSort: Cannot write to data block\n");
        return -1;
    }

   if( ES_NewFile(name,block) == 0){
       //printf("new file mallon ok\n");
       return 0;
   }else {
       //printf(" new file sigoura not ok\n");
       return -1;
   }
}

// Merge 2 sorted blocks

int ES_MergeSort(char* file1,char* file2, int fieldno, int perasma,int id) {


    //printf("Records per block %d\n",ES_SLOTS_FOR_RECORDS);

    //creating name for new file
    char name[20]="perasma-";
    char str[10];
    snprintf(str, 10, "%d", perasma);
    strcpy(name,strcat(name, str));
    strcpy(name,strcat(name,"-block-"));
    snprintf(str, 10, "%d", id);
    strcpy(name,strcat(name, str));
    //printf("name is: %s\n",name);

    //Creating and opening new file

    int file_desc;

    if(Sorted_CreateFile(name)==-1){
        printf("ES_NewFile: Error creating new file\n");
        return -1;
    }

    file_desc=ES_OpenFile(name);

    if(file_desc==-1){
        printf("ES_NewFile: Error opening new file\n");
        return -1;
    }


    // Opening files to merge and getting num of blocks
    int file_desc_1 = ES_OpenFile(file1);

    if(file_desc_1==-1){
        printf("ES_Merge_sort: Error opening first file\n");
        return -1;
    }

    int totalBlockNum1;
    if ((totalBlockNum1 = BF_GetBlockCounter(file_desc_1)) == -1) {
        BF_PrintError(" HT_InsertToDataBlock: BF_GetBlockCounter error\n");
        Sorted_CloseFile(file_desc);
        return -1;
    }

    int file_desc_2 = ES_OpenFile(file2);

    if(file_desc_2==-1){
        printf("ES_Merge_sort: Error opening second file\n");
        Sorted_CloseFile(file_desc);
        Sorted_CloseFile(file_desc_1);
        return -1;
    }

    int totalBlockNum2;
    if ((totalBlockNum2 = BF_GetBlockCounter(file_desc_2)) == -1) {
        BF_PrintError(" HT_InsertToDataBlock: BF_GetBlockCounter error\n");
        Sorted_CloseFile(file_desc);
        Sorted_CloseFile(file_desc_1);
        Sorted_CloseFile(file_desc_2);
        return -1;
    }



    void *block1;
    void *block2;

    Record record1, record2;

    DataBlock_info b1, b2;


    int k = 1, l = 1; // block iteration
    int m = 0, n = 0; // record iteration

    while(k<totalBlockNum1&&l<totalBlockNum2){

        if(k<totalBlockNum1){
        //printf("Reading block no %d out of %d from first file\n",k,totalBlockNum1);
        if (BF_ReadBlock(file_desc_1, k, &block1) < 0) {
            BF_PrintError(" ES_MergeSort: Cannot read data block 1\n");
            Sorted_CloseFile(file_desc);
            Sorted_CloseFile(file_desc_1);
            Sorted_CloseFile(file_desc_2);
            return -1;
        }}

        if(l<totalBlockNum2){
        //printf("Reading block no %d out of %d from second file\n",l,totalBlockNum2);
        if (BF_ReadBlock(file_desc_2, l, &block2) < 0) {
            BF_PrintError(" ES_MergeSort: Cannot read data block 2\n");
            Sorted_CloseFile(file_desc);
            Sorted_CloseFile(file_desc_1);
            Sorted_CloseFile(file_desc_2);
            return -1;
        }}

        memcpy(&b1, block1, sizeof(DataBlock_info));
        memcpy(&b2, block2, sizeof(DataBlock_info));


        while(m < b1.entriesNum && n < b2.entriesNum){


            memcpy(&record1, (block1 + sizeof(DataBlock_info) + m * sizeof(Record)), sizeof(Record));
            memcpy(&record2, (block2 + sizeof(DataBlock_info) + n * sizeof(Record)), sizeof(Record));

            if (fieldno == 0){
                if (record1.id < record2.id) {
                    Sorted_InsertEntry(file_desc,record1);
                    m++;
                }
                else {
                    Sorted_InsertEntry(file_desc,record2);
                    n++;
                }
            }
            else if (fieldno == 1){

                if (strcmp(record1.name, record2.name) == 0){
                    if (record1.id < record2.id) {
                        Sorted_InsertEntry(file_desc,record1);
                        m++;
                    }
                    else {
                        Sorted_InsertEntry(file_desc,record2);
                        n++;
                    }
                }
                else if (strcmp(record1.name, record2.name) < 0){
                    Sorted_InsertEntry(file_desc,record1);
                    m++;
                }
                else {
                    Sorted_InsertEntry(file_desc,record2);
                    n++;
                }
            }
            else if (fieldno == 2){

                if (strcmp(record1.surname, record2.surname) == 0){
                    if (record1.id < record2.id) {
                        Sorted_InsertEntry(file_desc,record1);
                        m++;
                    }
                    else {
                        Sorted_InsertEntry(file_desc,record2);
                        n++;
                    }
                }
                else if (strcmp(record1.surname, record2.surname) < 0){
                    Sorted_InsertEntry(file_desc,record1);
                    m++;
                }
                else {
                    Sorted_InsertEntry(file_desc,record2);
                    n++;
                }
            }
            else if (fieldno == 3){

                if (strcmp(record1.city, record2.city) == 0){
                    if (record1.id < record2.id) {
                        Sorted_InsertEntry(file_desc,record1);
                        m++;
                    }
                    else {
                        Sorted_InsertEntry(file_desc,record2);
                        n++;
                    }
                }
                else if (strcmp(record1.city, record2.city) < 0){
                    Sorted_InsertEntry(file_desc,record1);
                    m++;
                }
                else {
                    Sorted_InsertEntry(file_desc,record2);
                    n++;
                }
            }
            else{
                printf("ES_MergeSort: Wrong type of field\n");
                Sorted_CloseFile(file_desc);
                Sorted_CloseFile(file_desc_1);
                Sorted_CloseFile(file_desc_2);
                return -1;
            }

        }

     if(m == b1.entriesNum && k==totalBlockNum1 - 1){
         k=totalBlockNum1;
     } else if(n == b2.entriesNum && l==totalBlockNum2 -1){
         l=totalBlockNum2;
     } else if(m==b1.entriesNum){
         m=0;
         k++;
     }else if(n==b2.entriesNum){
         n=0;
         l++;
     }
    }




    while(m < b1.entriesNum && k<totalBlockNum1){
        memcpy(&record1, (block1 + sizeof(DataBlock_info) + m * sizeof(Record)), sizeof(Record));
        Sorted_InsertEntry(file_desc,record1);
        m++;
        if(m==b1.entriesNum)k++;
    }



    while(n < b2.entriesNum && l<totalBlockNum2){
        memcpy(&record2, (block2 + sizeof(DataBlock_info) + n * sizeof(Record)), sizeof(Record));
        Sorted_InsertEntry(file_desc,record2);
        n++;
        if(n==b2.entriesNum)l++;
    }




           //ES_PrintFile(file_desc);

    Sorted_CloseFile(file_desc);
    Sorted_CloseFile(file_desc_1);
    Sorted_CloseFile(file_desc_2);

    if(remove(file1)==0 && remove(file2)==0){
        //printf("Temporary files %s and %s deleted successfully\n",file1,file2);
    } else printf("Error while removing temporary files\n");

 return 0;



}

// Print block of Records

void ES_PrintBlockOfRecords(int fileDesc, int blocknum) {

    void *block;

    if (BF_ReadBlock(fileDesc, blocknum, &block) < 0) {
        BF_PrintError(" ES_PrintBlockOfRecords: Cannot read data block 1\n");
    }

    DataBlock_info info;
    memcpy(&info, block, sizeof(DataBlock_info));
    printf("\n   ID            Name                   Surname                 city\n");

    printf("___________________________________________________________________________________\n");
    printf("Block number %d\n\n",blocknum);

    Record record;
    int i = 0;
    for (i = 0; i < info.entriesNum; i++) {

    memcpy(&record, (block + sizeof(DataBlock_info) + i * sizeof(Record)), sizeof(Record));

    printf("%10d. \t%-20s \t%-20s \t%-25s \n",record.id,record.name, record.surname, record.city);
}
    printf("___________________________________________________________________________________\n\n");


}

void ES_PrintBlock(void* block) {


    DataBlock_info info;
    memcpy(&info, block, sizeof(DataBlock_info));
    printf("\n   ID            Name                   Surname                 city\n");

    printf("___________________________________________________________________________________\n");
    printf("Just a block \n\n");

    Record record;
    int i = 0;
    for (i = 0; i < info.entriesNum; i++) {

        memcpy(&record, (block + sizeof(DataBlock_info) + i * sizeof(Record)), sizeof(Record));

        printf("%10d. \t%-20s \t%-20s \t%-25s \n",record.id,record.name, record.surname, record.city);
    }
    printf("___________________________________________________________________________________\n\n");


}

void ES_PrintFile(int fileDesc){




    int totalBlockNum;
    if ((totalBlockNum = BF_GetBlockCounter(fileDesc)) == -1) {
        BF_PrintError(" ES_PrintFile: BF_GetBlockCounter error\n");

    }


    int i;
    for (i = 1; i < totalBlockNum; i++) {

         ES_PrintBlockOfRecords(fileDesc, i);

    }




}


int ES_NewFile(char* FileName, void* block){

    int file_desc;
    void* block2 = malloc(sizeof(BLOCK_SIZE));

    if(Sorted_CreateFile(FileName)==-1){
        printf("ES_NewFile: Error creating new file\n");
        return -1;
    }

    file_desc=ES_OpenFile(FileName);

    if(file_desc==-1){
        printf("ES_NewFile: Error opening new file\n");
        return -1;
    }

    //ES_info* info;
    //info = ES_ReturnInfo(file_desc);
    //DataBlock_info b1;

    ES_NewDataBlock(file_desc,1);

    // read block and get info
    if (BF_ReadBlock(file_desc, 1, &block2) < 0) {
        BF_PrintError(" ES_NewFile: Cannot read data block\n");
        return -1;
    }

    memcpy(block2,block,BLOCK_SIZE);


    //printf("meta tin antigrafi\noriginal block: \n");
    //ES_PrintBlock(block);
    //printf("block2:\n");
    //ES_PrintBlock(block2);




    //block2=block;

    if (BF_WriteBlock(file_desc, 1) < 0) {
        BF_PrintError(" ES_NewFile: Cannot write to data block\n");
        return -1;
    }

    //info->type=1;



   // ES_PrintBlockOfRecords(file_desc,1);

    Sorted_CloseFile(file_desc);

    return 0;


}

int ES_PairFiles(char* template, int num, int fieldNo, int perasma){

    int i;

    if(num%2==0){

        for (i = 1; i < num; i=i+2) {
                char name1[20];
                strcpy(name1,template);
                char str[10];
                snprintf(str, 10, "%d", i);
                strcpy(name1, strcat(name1, str));
                //printf("name is: %s\n", name1);

                char name2[20];
                strcpy(name2,template);
                snprintf(str, 10, "%d", i+1);
                strcpy(name2, strcat(name2, str));
                //printf("name is: %s\n", name2);

                ES_MergeSort(name1,name2,fieldNo,perasma,(i/2)+1);
            }

    }
    else{

        for ( i = 1; i < num-1; i=i+2) {
            char name1[20];
            strcpy(name1,template);
            char str[10];
            snprintf(str, 10, "%d", i);
            strcpy(name1, strcat(name1, str));
            //printf("name is: %s\n", name1);

            char name2[20];
            strcpy(name2,template);
            snprintf(str, 10, "%d", i+1);
            strcpy(name2, strcat(name2, str));
            //printf("name is: %s\n", name2);

            ES_MergeSort(name1,name2,fieldNo,perasma,(i/2)+1);
        }
        char name1[20];
        strcpy(name1,template);
        char str[10];
        snprintf(str, 10, "%d", num);
        strcpy(name1, strcat(name1, str));
        //printf("name is: %s\n", name1);


        char name[20]="perasma-";
        char str2[10];
        snprintf(str2, 10, "%d", perasma);
        strcpy(name,strcat(name, str2));
        strcpy(name,strcat(name,"-block-"));
        snprintf(str2, 10, "%d", num/2+1);
        strcpy(name,strcat(name, str2));
        //printf("name is: %s\n",name);

        rename(name1,name);
        //printf("old name: %s   new name: %s\n",name1,name);

    }

    return 0;
}

int ES_FinalizeHeap(char* filename, int fieldno){
    int filedesc = ES_OpenFile(filename);
    if(filedesc==-1){
        printf("Error opening final file\n");
        return -1;
    }
    ES_info* info = ES_ReturnInfo(filedesc);
    info->fieldNo=fieldno;
    info->type=1;

    void* block = malloc(sizeof(BLOCK_SIZE));
    if (BF_ReadBlock(filedesc, 0, &block) < 0) {
        BF_PrintError(" ES_FinalizeHeap: Cannot read info block\n");
        return -1;
    }
    memcpy(block, info, sizeof (ES_info));

    printES_info(info);

    if(Sorted_CloseFile(filedesc)==-1){
        printf("Error closing final file\n");
        return -1;
    }
    return 0;
}


int ES_CheckRecord(Record rec,int field, void* value){

    if(field==0){
        const int *val = value;
        if(rec.id > *val){
            return -1;
        }else if (rec.id < *val){
            return 1;
        }else{
            return 0;
        }

    } else if(field==1){

        //return strcmp(rec.name,value);
        if(strcmp(rec.name,value)>0){
            //printf("Searching for %s and found %s -> city more than value\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)>0 = -1\n ",strcmp(rec.surname,value));
            return -1;
        }else if (strcmp(rec.name,value)<0){
            //printf("Searching for %s and found %s -> city less than value\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)<0 = 1\n ",strcmp(rec.surname,value));
            return 1;
        }else{
            //printf("Searching for %s and found %s -> matching true\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)==0 = 0\n ",strcmp(rec.surname,value));
            return 0;
        }

    }else if(field==2){

        //return strcmp(rec.surname,value);
        if(strcmp(rec.surname,value)>0){
            //printf("Searching for %s and found %s -> city more than value\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)>0 = -1\n ",strcmp(rec.surname,value));
            return -1;
        }else if (strcmp(rec.surname,value)<0){
            //printf("Searching for %s and found %s -> city less than value\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)<0 = 1\n ",strcmp(rec.surname,value));
            return 1;
        }else{
            //printf("Searching for %s and found %s -> matching true\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)==0 = 0\n ",strcmp(rec.surname,value));
            return 0;
        }

    }else if(field==3){


        //return strcmp(rec.city,value);

        if(strcmp(rec.city,value)>0){
            //printf("Searching for %s and found %s -> city more than value\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)>0 = -1\n ",strcmp(rec.surname,value));
            return -1;
        }else if (strcmp(rec.city,value)<0){
            //printf("Searching for %s and found %s -> city less than value\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)<0 = 1\n ",strcmp(rec.surname,value));
            return 1;
        }else{
            //printf("Searching for %s and found %s -> matching true\n",value,rec.city);
            //printf("strcmp(rec.city,value): %d, strcmp(rec.city,value)==0 = 0\n ",strcmp(rec.surname,value));
            return 0;
        }

    }else{
        printf("Wrong field number\n");
    }
return 0;
}


int ES_searchblock(int filedesc,int fieldNo,void* value,int blocknum){
    //printf("Searching block no: %d\n",blocknum);
    void *block;
    int matched=0;
    if (BF_ReadBlock(filedesc, blocknum, &block) < 0) {
        BF_PrintError(" ES_PrintBlockOfRecords: Cannot read data block 1\n");
    }

    DataBlock_info info;
    memcpy(&info, block, sizeof(DataBlock_info));
    Record record;
    int i = 0;
    for (i = 0; i < info.entriesNum; i++) {

        memcpy(&record, (block + sizeof(DataBlock_info) + i * sizeof(Record)), sizeof(Record));
        int result = ES_CheckRecord(record,fieldNo,value);
        if(result==0){
            matched++;
            //printRecord(record);
        }
    }
    return  matched;
}

int ES_splitfile(int fileDesc,int fieldNo, void* value,int upper,int lower){
    //printf("Split file: upper: %d lower: %d\n",upper,lower);
    Record record;
    void *block;
    int middle=((upper+lower)/2);
    DataBlock_info *info=malloc(sizeof(DataBlock_info));
    if(upper!=lower&&(upper-lower)!=1) {
    //printf("Going to middle block no: %d\n",middle);
        if (BF_ReadBlock(fileDesc, middle, &block) < 0) {
            BF_PrintError("Could not read block");
            return -1;
        }
        search_blocks++;
        ES_GetDataBlockInfo(fileDesc,info,middle);
        memcpy(&record, block + sizeof(DataBlock_info), sizeof(Record));
        int result = ES_CheckRecord(record, fieldNo, value);
        //printf("Result for first record is: %d\n",result);

        if(result<0){
            //if(upper!=middle)
                upper=middle;
            //else upper--;
            return ES_splitfile(fileDesc,fieldNo,value,upper,lower);

        }else if(result==0){
            //psakse gitonika block

            int matched=2;
            int less=middle-1;
            int sum;
            //printf("ta epomena\n");
            while(matched>0){

                matched=ES_searchblock(fileDesc,fieldNo,value,middle);
                sum=sum+matched;
                middle++;
            }
            matched=2;
            //printf("ta proigoumena\n");
            while(matched>0){
                matched=ES_searchblock(fileDesc,fieldNo,value,less);
                sum=sum+matched;
                less--;
            }
            //printf("Matched in records:%d\n",sum);
            return sum;


        }else if(result>0) {
            memcpy(&record, block + sizeof(DataBlock_info) + (info->entriesNum - 1) * sizeof(Record), sizeof(Record));
            result = ES_CheckRecord(record, fieldNo, value);
            if (result<0){
                //printf("eimaste sto sosto monadiko block pou theloume\n");
                //eimaste sto sosto monadiko block pou theloume
                int matched=ES_searchblock(fileDesc,fieldNo,value,middle);
                //printf("Matched in records:%d\n",matched);
                return matched;
            }else if(result==0){
               // printf("psakse auto kai ta megalitera gitonika\n");

                //psakse auto kai ta megalitera gitonika
                int matched=2;
                int sum;
                while(matched>0){
                    matched=ES_searchblock(fileDesc,fieldNo,value,middle);
                    sum=sum+matched;
                    middle++;
                }
                //printf("Matched in records:%d\n",sum);
                return sum;
            }else if(result>0){
                //if (lower!= middle)
                    lower=middle;
                //else lower++;
                return ES_splitfile(fileDesc,fieldNo,value,upper,lower);
            }
        }
    } else if((upper-lower)==1) {
       // printf("just search both blocks\n");
        return ES_searchblock(fileDesc,fieldNo,value,upper) + ES_searchblock(fileDesc,fieldNo,value,lower);
    }
        else return ES_searchblock(fileDesc,fieldNo,value,upper);

return 0;

}
