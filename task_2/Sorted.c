//
// Created by katerina on 17/1/2017.
//

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "Sorted.h"
#include "BF.h"
#include "ES_info.h"
#include "DataFunctions.h"
#include "Record.h"

int search_blocks;


int Sorted_CreateFile(const char *fileName  /* όνομα αρχείου */ ){

    // Create heap file
   // printf("Sorted_CreateFile\n");

    FILE *file;
    int fileDesc;
    char* block;
    int sorted;
    sorted=0;
    ES_info* info;

    if( (file=fopen(fileName,"r"))){				//an yparxei to dosmeno arxeio
        printf("Warning: %s file already exists and it will be overwritten.\n",fileName);
        fclose(file);
    }
    if( BF_CreateFile(fileName)<0 ){
        BF_PrintError("Could not create file");
        return -1;
    }
    if( (fileDesc=BF_OpenFile(fileName))<0 ){
        BF_PrintError("Could not open file");
        return -1;
    }
    if(BF_AllocateBlock(fileDesc)<0){
        BF_PrintError("Could not allocate block");
        return -1;
    }
    if( BF_ReadBlock(fileDesc,0,(void*)&block)<0 ){
        BF_PrintError("Could not read block");
        return -1;
    }

    info = newES_info(sorted,fileDesc,0);
    memcpy(block,info,sizeof(ES_info));
    if( BF_WriteBlock(fileDesc,0)<0 ){
        BF_PrintError("Could not write block");
        return -1;
    }

    //printES_info(info);

    if( BF_CloseFile(fileDesc)<0 ){
        BF_PrintError("Could not close file");
        return -1;
    }


    return 0;

}

int Sorted_OpenFile( const char *fileName /* όνομα αρχείου */ ){

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
        printf("Sorted File.\n");
        info->fileDesc = fileDesc;
        memcpy(block,info,sizeof(ES_info));
        if( BF_WriteBlock(fileDesc,0)<0 ){
            BF_PrintError("Could not write block");
            return -1;
        }
        return fileDesc;
    }else{
        printf("Not Sorted File.\n");
        Sorted_CloseFile(fileDesc);
            return -1;

        }

}

int Sorted_CloseFile( int fileDesc /* αναγνωριστικός αριθμός ανοίγματος αρχείου */ ){

    void* block;
    ES_info* info;

    info = ES_ReturnInfo(fileDesc);

    //printES_info(info);

    if( BF_ReadBlock(fileDesc,0,&block)<0 ){
        BF_PrintError("Could not read block");
        return -1;
    }



    // update filedesc
    //memcpy(info,block,sizeof(ES_info));
    info->fileDesc = -1;

    //printES_info(info);

    memcpy(block,info,sizeof(ES_info));
    if( BF_WriteBlock(fileDesc,0)<0 ){
        BF_PrintError("Could not write block");
        return -1;
    }
    //printf("write to block?\n");
    // close file
    if( BF_CloseFile(fileDesc)<0 ){
        BF_PrintError("Could not close file");
        return -1;
    }

    //printf("file closed\n");
    //free(block);
    return 0;

}

int Sorted_InsertEntry( int fileDesc, /* αναγνωριστικός αριθμός ανοίγματος αρχείου */
                        Record record /* δομή που προσδιορίζει την εγγραφή */ ){




    ES_info* info = ES_ReturnInfo(fileDesc);

    int totalBlockNum;
    if ((totalBlockNum = BF_GetBlockCounter(fileDesc)) == -1) {
        BF_PrintError(" ES_InsertEntry: BF_GetBlockCounter error\n");
        return -1;
    }

    if(totalBlockNum==1){
        if (ES_NewDataBlock(fileDesc, totalBlockNum) < 0) {
            BF_PrintError("ES_InsertEntry: Could not create new data block");
            return -1;
        }
        if ((totalBlockNum = BF_GetBlockCounter(fileDesc)) == -1) {
            BF_PrintError(" ES_InsertEntry: BF_GetBlockCounter error\n");
            return -1;
        }

        return ES_InsertToDataBlock(*info,totalBlockNum-1,record);

    }

    return ES_InsertToDataBlock(*info,totalBlockNum-1,record);


}

int Sorted_SortFile( const char *fileName, /* όνομα αρχείου */
                      int fieldNo /* αύξων αριθμός πεδίου προς σύγκριση */) {

    //printf("Function to sort file\n");

    /// 1.Sort each block separately and create a file for each block
    //printf("Sort each block separately\n");

    int fileDesc = ES_OpenFile(fileName);

    int totalBlockNum;
    if ((totalBlockNum = BF_GetBlockCounter(fileDesc)) == -1) {
        BF_PrintError(" HT_InsertToDataBlock: BF_GetBlockCounter error\n");
        return -1;
    }

    //Sort each block and create file
    int i;
    for (i = 1; i < totalBlockNum; i++) {
    //printf("Unsorted block \n");
   // ES_PrintBlockOfRecords(fileDesc, i);
        ES_BlockBubbleSort(fileDesc, i, fieldNo);
    //printf("Sorted block\n");
   // ES_PrintBlockOfRecords(fileDesc, i);
    }
    printf("Block Bubble Sorting completed\n");
    Sorted_CloseFile(fileDesc);
    printf("_____________________________________________________________________\n");



    /// 2. Merge created files in pairs

    int perasma =1;
    int more_files;
    more_files = totalBlockNum-1;

    ES_PairFiles("sorted-block-",more_files,fieldNo, perasma);
    printf("Perasma no: %d\n",perasma);

    if(more_files%2==0) more_files=more_files/2;
    else more_files = more_files/2 + 1;

    while(more_files>1){
        char name[20]="perasma-";
        char str2[10];
        snprintf(str2, 10, "%d", perasma);
        strcpy(name,strcat(name, str2));
        strcpy(name,strcat(name,"-block-"));
        //printf("name is: %s\n",name);
        perasma++;
        printf("Perasma no: %d\n",perasma);
        ES_PairFiles(name,more_files,fieldNo, perasma);
        if(more_files%2==0) more_files=more_files/2;
        else more_files = more_files/2 + 1;
        if(more_files==1){
            char name[20]="perasma-";
            char str2[10];
            snprintf(str2, 10, "%d", perasma);
            strcpy(name,strcat(name, str2));
            strcpy(name,strcat(name,"-block-1"));
            //printf("name is: %s\n",name);
            snprintf(str2, 10, "%d", fieldNo);
            char name2[20];
            strcpy(name2,fileName);
            strcat(name2,"Sorted");
            strcat(name2,str2);
            rename(name,name2);
            //printf("Old name: %s\nNew name: %s\n",name,name2);

            if(ES_FinalizeHeap(name2,fieldNo)==-1){
                printf("Error finalizing sorted heap file\n");
                return -1;
            }
        }

    }



    return 0;

}

int Sorted_checkSortedFile( const char *fileName, /* όνομα αρχείου */
                             int fieldNo /* αύξων αριθμός πεδίου προς σύγκριση */ ){


    //printf("Function to check if file sorted according to field number\n");

    int fileDesc = ES_OpenFile(fileName);

    if(fileDesc==-1){
        printf("Sorted_checkSortedFile: Error opening file\n");
    }

    ES_info* header;

    header=ES_ReturnInfo(fileDesc);
    if(Sorted_CloseFile(fileDesc)==-1){
        printf("Error closing file\n");
        return -1;
    }
    if(header->type==1)
        if(header->fieldNo==fieldNo)
        {
            printf("File %s is sorted by field no %d\n",fileName,fieldNo);
            return 0;
        }
        else
        {
            printf("File %s is sorted but not by field no %d\n",fileName,fieldNo);

            return -1;
        }
    else {

        printf("File %s is not sorted\n",fileName);

        return -1;
    }

}

void Sorted_GetAllEntries( int fileDesc, /* αναγνωριστικός αριθμός ανοίγματος αρχείου */
                           int fieldNo, /* αύξων αριθμός πεδίου προς σύγκριση */
                           void *value /* τιμή του πεδίου προς σύγκριση */ ){

    search_blocks=0;

    if(fieldNo==0){
        const int *val = value;
        printf("Searching for %d in field %d\n",(*val),fieldNo);}
    else
    { const char *val2 = value;
        printf("Searching for %s in field %d\n",val2,fieldNo);}


        if(value==NULL){
            // print everything
            ES_PrintFile(fileDesc);
        }else {

            int totalBlockNum;
            if ((totalBlockNum = BF_GetBlockCounter(fileDesc)) == -1) {
                BF_PrintError(" ES_InsertEntry: BF_GetBlockCounter error\n");
            }

            int found = ES_splitfile(fileDesc,fieldNo,value,totalBlockNum-1,1);
            printf("Records found: %d\n",found);
            printf("Blocks read: %d\n",search_blocks);
        }




}