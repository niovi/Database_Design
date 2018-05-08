#include <memory.h>
#include <malloc.h>
#include "BF.h"
#include "hash.h"
#include "DataFunctions.h"
#include "hashfunc.h"
#include "HT_info.h"


int HT_CreateIndex(char *fileName, char attrType, char* attrName, int attrLength, int buckets){
    /* Add your code here */
    BF_Init();
    if( attrType != 'i' && attrType != 'c' ) {	//attrType only i or c

        printf("HT_CreateIndex: Wrong attrType!\n");
        return -1;
    }

    printf("HT_CreateIndex: attrType: accepted\n");
        if (attrName=="id") {
            printf("HT_CreateIndex: attrName: id\n");
            if(attrLength!=2 || attrType != 'i'){
                printf("HT_CreateIndex: Wrong combination of attrName, attrLength and attrType 1!\n");
                return -1;
            }
        } else if (attrName=="name") {
            if (attrLength!=4 || attrType != 'c') {
                printf("HT_CreateIndex: Wrong combination of attrName, attrLength and attrType 2!\n");
                return -1;
            }
        } else if (attrName=="surname") {
            if (attrLength!=7 || attrType != 'c') {
                printf("HT_CreateIndex: Wrong combination of attrName, attrLength and attrType 3!\n");
                return -1;
            }
        }else if (attrName=="city") {
            if(attrLength!=4 || attrType != 'c') {
                printf("HT_CreateIndex: Wrong combination of attrName, attrLength and attrType 4!\n");
                return -1;
            }
         else{
                printf("HT_CreateIndex: Wrong attrName!\n");
                return -1;
            }

        }



    int fileDesc;
    if (BF_CreateFile(fileName)<0){
        BF_PrintError(" HT_CreateIndex: Cannot create BF file!\n");
        return -1;
    }
    if((fileDesc=BF_OpenFile(fileName)) < 0){
        BF_PrintError(" HT_CreateIndex: Cannot open BF file!\n");
        return  -1;

    }



    // create file index
    HT_info* fileInfo;
    fileInfo = newHT_info(1, fileDesc, attrType, attrName, attrLength, buckets);
    // type: 1 for static hashing, 2 for extendible hashing


    //allocate info block
    void *block;
    if (BF_AllocateBlock(fileDesc)<0){
        BF_PrintError(" HT_CreateIndex: Cannot allocate new block for index!\n");
        BF_CloseFile(fileDesc);
        return -1;
    }

    //initialize header info at block 0
    if(BF_ReadBlock(fileDesc,0,&block)<0) {
        BF_PrintError(" HT_CreateIndex: Cannot read from block!\n");
        BF_CloseFile(fileDesc);
        return -1;
    }

    memcpy(block,fileInfo, sizeof(HT_info));

    if (BF_WriteBlock(fileDesc,0)<0){
        BF_PrintError(" HT_CreateIndex: Cannot write to block!\n");
        BF_CloseFile(fileDesc);
        return -1;
    }

    printHT_info(fileInfo);


    //  ADDRESS TABLE
    if(HT_NewAddrTableBlock(fileDesc, buckets)<0){
        printf("Cannot create address table\n");
        return -1;
    }



    // allocate blocks for buckets
    int j;
    for (j=0; j<buckets; j++){
        if (HT_NewDataBlock(fileDesc,BF_GetBlockCounter(fileDesc))<0){
            BF_PrintError(" HT_CreateIndex: Cannot allocate new block for bucket!\n");
            BF_CloseFile(fileDesc);
            return -1;
        }
    }

    // close file
    if (BF_CloseFile(fileDesc)<0){
        BF_PrintError("HT_CreateIndex: Cannot close BF file!\n");
        return -1;
    }

    // free memory
    deleteHT_info(fileInfo);
    return 0;
}


HT_info* HT_OpenIndex(char *fileName) {
    /* Add your code here */
    BF_Init();
    int fileDesc;
    if((fileDesc=BF_OpenFile(fileName)) < 0){
        BF_PrintError(" HT_OpenIndex: Cannot open BF file!\n");
        return  NULL;
    }

    void *block;
    // Get file index
    if(BF_ReadBlock(fileDesc,0,&block)<0) {
        BF_PrintError(" HT_OpenIndex: Cannot read first block!\n");
        BF_CloseFile(fileDesc);
        return NULL;
    }
    HT_info* fileInfo = malloc(sizeof(HT_info));
    memcpy(fileInfo,block,sizeof(HT_info));
    if (fileInfo->type != 1){
        printf(" HT_OpenIndex: Not a Static Hashing file!\n");
        BF_CloseFile(fileDesc);
        return NULL;
    }
    fileInfo->fileDesc =fileDesc;
    printf("fileDesc:%d for file %s\n", fileInfo->fileDesc, fileName);
    return fileInfo;

    
} 


int HT_CloseIndex(HT_info* header_info ) {
    /* Add your code here */
    int fileDesc = header_info->fileDesc;
    void *block;
    if (BF_ReadBlock(fileDesc,0,&block)<0){
        BF_PrintError(" HT_CloseIndex: Cannot read first block!\n");
        BF_CloseFile(fileDesc);
        return -1;
    }
    header_info->fileDesc=-1;
    memcpy(block,header_info, sizeof(HT_info));
    if (BF_WriteBlock(fileDesc,0)<0){
        BF_PrintError(" HT_CloseIndex: Cannot write to block!\n");
        BF_CloseFile(fileDesc);
        return -1;
    }
    if (BF_CloseFile(fileDesc)<0){
        BF_PrintError(" HT_CloseIndex: Cannot close file!\n");
        return -1;
    }

    //deleteHT_info(header_info); free header_info in main
    return 0;
    
}

int HT_InsertEntry(HT_info header_info, Record record) {
    /* Add your code here */

    //printf("HT_InsertEntry: start!\n");
    //printf("Buckets: %ld\n", header_info.numBuckets);

    //create hash for record
    int hash =0;
    if (header_info.attrType == 'c') {
        if (!strcmp(header_info.attrName, "name")) {
            hash= HT_HashFunction(record.name, header_info.numBuckets);
            //hash=strlen(record.name)%header_info.numBuckets;
            //hash= (unsigned int) Char_Hash_Function(record.name, header_info.attrName);
        } else if (!strcmp(header_info.attrName, "surname")) {
            hash= HT_HashFunction(record.surname, header_info.numBuckets);
            //hash=strlen(record.surname)%header_info.numBuckets;
            //hash= (unsigned int) Char_Hash_Function(record.surname, header_info.attrName);
        } else if (!strcmp(header_info.attrName, "city")) {
            hash= HT_HashFunction(record.city, header_info.numBuckets);
            //hash=strlen(record.city)%header_info.numBuckets;
            //hash= (unsigned int) Char_Hash_Function(record.city, header_info.attrName);
        }
    }else if (header_info.attrType == 'i') {
            if (!strcmp(header_info.attrName, "id")) {
               // hash=record.id%header_info.numBuckets;
                hash= record.id%header_info.numBuckets;
            }
    } else {
            printf("Wrong attrType!\n");
            return -1;
    }

        // hash = mod_hash(&(record.id), &(record.name),&(record.surname),&(record.city),header_info.numBuckets);

        // Calculate the address table
        //int size = header_info.addr_size;
        AddressEntry *addressTable = malloc(header_info.numBuckets * sizeof(AddressEntry));
        if(HT_GetAddrTable(header_info.fileDesc,addressTable)<0){
            printf("HT_InsertEntry: Cannot get address table!\n");
            return -1;
        }

        //find bucket using address table and hash
        //int bucket = addressTable[hash % header_info.numBuckets];
        //int bucket = addressTable[hash].last_block;
        printf("Record has to go to bucket no: %d\n",hash+1);
        printf("Record has to go to block no: %d\n",addressTable[hash].last_block);

    if (HT_InsertToDataBlock(header_info,hash+1, addressTable[hash].last_block, record) < 0) {
            BF_PrintError(" HT_InsertEntry: Cannot insert record to block!\n");
            return -1;
        }

        free(addressTable);
        return 0;
    }






    int HT_GetAllEntries(HT_info header_info, void *value) {
        /* Add your code here */





        void *block = NULL;
        printf("searching for: %s \n", (char*)value);
        Record record;
        int i,j;
        int matched=0;

        if (value == NULL){
            // print everything
            int totalBlockNumber;
            if ((totalBlockNumber = BF_GetBlockCounter(header_info.fileDesc)) == -1) {
                BF_PrintError(" HT_GetAllEntries: BF_GetBlockCounter error\n");
                return -1;
            }
            for (i = header_info.addr_size+1; i < totalBlockNumber; ++i) {
                if (BF_ReadBlock(header_info.fileDesc, i, &block) < 0) {
                    BF_PrintError(" HT_GetAllEntries: Cannot read block\n");
                    return -1;
                }
                DataBlock_info block_info;

                memcpy(&block_info, block, sizeof(DataBlock_info));
                for (j = 0; j < block_info.entriesNum; ++j) {
                    memcpy(&record, block + sizeof(DataBlock_info) + (j * sizeof(record)), sizeof(record));
                    printRecord(record);

                }
            }


        }
        else{
            int hash;
            if (header_info.attrType=='i'){hash=atoi(value)%header_info.numBuckets;}
            else hash= HT_HashFunction(value, header_info.numBuckets);
            // Calculate the address table
            //int size = header_info.addr_size;
            AddressEntry *addressTable = malloc(header_info.numBuckets * sizeof(AddressEntry));
            if(HT_GetAddrTable(header_info.fileDesc,addressTable)<0){
                printf("HT_GetAllEntries: Cannot get address table!\n");
                return -1;
            }
            //find bucket using address table and hash
            //int bucket = addressTable[hash % header_info.numBuckets];
            int bucket = hash+1;
            printf("Going to bucket %d\n",bucket);
            if (BF_ReadBlock(header_info.fileDesc, addressTable[hash].first_block, &block) < 0) {
                BF_PrintError(" HT_GetAllEntries: Cannot read block\n");
                return -1;
            }
            DataBlock_info block_info;
            memcpy(&block_info, block, sizeof(DataBlock_info));
            printf("block info: %d entries %d next overflow block\n",block_info.entriesNum,block_info.overflow);
            for (j = 0; j < block_info.entriesNum; ++j) {
                memcpy(&record, block + sizeof(DataBlock_info) + (j * sizeof(record)), sizeof(record));
                //printRecord(record);
                if (!strcmp(header_info.attrName, "id")) {

                    if (record.id == *(int *) value) {
                        matched++;
                        printf("Match found!\n");
                        printRecord(record);
                    }

                } else if (!strcmp(header_info.attrName, "name")) {

                    if (!strcmp(record.name, (char *) value)) {
                        matched++;
                        printf("Match found!\n");
                        printRecord(record);
                    }

                } else if (!strcmp(header_info.attrName, "surname")) {

                    if (!strcmp(record.surname, (char *) value)) {
                        matched++;
                        printf("Match found!\n");
                        printRecord(record);
                    }

                } else if (!strcmp(header_info.attrName, "city")) {

                    if (!strcmp(record.city, (char *) value)) {
                        matched++;
                        printf("Match found!\n");
                        printRecord(record);
                    }

                }



            }
            while(block_info.overflow>0){

                printf("searching overflow bucket %d\n", block_info.overflow);
                if (BF_ReadBlock(header_info.fileDesc, block_info.overflow, &block) < 0) {
                    BF_PrintError(" HT_GetAllEntries: Cannot read block\n");
                    return -1;
                }

                memcpy(&block_info, block, sizeof(DataBlock_info));
                for (j = 0; j < block_info.entriesNum; ++j) {
                    memcpy(&record, block + sizeof(DataBlock_info) + (j * sizeof(record)), sizeof(record));
                    //printRecord(record);


                    if (!strcmp(header_info.attrName, "id")) {

                        if (record.id == *(int *) value) {
                            matched++;
                            printf("Match found!\n");
                            printRecord(record);
                        }

                    } else if (!strcmp(header_info.attrName, "name")) {

                        if (!strcmp(record.name, (char *) value)) {
                            matched++;
                            printf("Match found!\n");
                            printRecord(record);
                        }

                    } else if (!strcmp(header_info.attrName, "surname")) {

                        if (!strcmp(record.surname, (char *) value)) {
                            matched++;
                            printf("Match found!\n");
                            printRecord(record);
                        }

                    } else if (!strcmp(header_info.attrName, "city")) {

                        if (!strcmp(record.city, (char *) value)) {
                            matched++;
                            printf("Match found!\n");
                            printRecord(record);
                        }

                    }


                }

            }
            printf("%d matching records found\n\n",matched);

        }




        return 0;

    }


int HT_HashStatistics(char* filename) {
    /* Add your code here */

    //open file
    //check if bf file
    //get header
    //check if ht file
    HT_info* header;
    header=HT_OpenIndex(filename);

    //print statistics




    int totalBlockNumber;
    if ((totalBlockNumber = BF_GetBlockCounter(header->fileDesc)) == -1) {
        BF_PrintError("block num ht get all");
        return -1;
    }
    printf("The total number of blocks of file %p is : %d\n", (void *) filename, totalBlockNumber);

    Bucket_info B_info;
    int i;

    int R_slots=(int)((BLOCK_SIZE-(sizeof (DataBlock_info)))/(sizeof(Record)));


    int min_records=R_slots,max_records=0,mean_records=0;
    int mean_blocks=0;
    int over_buckets=0;
    for (i=0;i<header->numBuckets;i++){
        B_info=HT_GetBucketInfo(header->fileDesc,i+1+header->addr_size);
        printf("Bucket no %d has %d records\n",i+1,B_info.records);
        if(B_info.records==-1&&B_info.blocks==-1){
            printf("Error getting info for bucket no %d\n",i);
            return -1;
        }
        mean_blocks = mean_blocks +B_info.blocks;
        mean_records= mean_records+B_info.records;
        if(B_info.blocks>1){
            printf("Bucket no %d has %d overflow blocks\n",i+1,B_info.blocks-1);
            over_buckets++;
        }
        if(B_info.records>max_records){
            max_records=B_info.records;
        }
        if(B_info.records<min_records){
            min_records=B_info.records;
        }
    }
    mean_blocks=(int)(mean_blocks/header->numBuckets);
    mean_records=(int)(mean_records/header->numBuckets);

    printf("Minimum number of records in a bucket: %d\n",min_records);
    printf("Maximum number of records in a bucket: %d\n",max_records);
    printf("Mean number of records in a bucket: %d\n",mean_records);
    printf("Mean number of blocks in a bucket: %d\n",mean_blocks);
    printf("Number of buckets that have overflow blocks: %d\n", over_buckets);

    if (HT_CloseIndex(header) < 0) {
        printf("Error closing file\n");
        return -1;
    }

    return 0;

}
