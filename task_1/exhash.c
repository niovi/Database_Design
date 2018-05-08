#include <malloc.h>
#include <math.h>
#include <string.h>
#include "BF.h"
#include "exhash.h"
#include "ExDataFunctions.h"
#include "exhashfunc.h"


int EH_CreateIndex(char *fileName, char* attrName, char attrType, int attrLength, int depth) {
    /* Add your code here */

    BF_Init();

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

    if (BF_CreateFile(fileName) < 0) {
        BF_PrintError("EH_CreateIndex: Cannot create BF file\n");
        return -1;
    }

    int fileDesc;
    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
        BF_PrintError("EH_CreateIndex: Cannot open BF file\n");
        return -1;
    }

    void* block;

    int i;

    //allocate info  block

        if (BF_AllocateBlock(fileDesc) < 0) {
            BF_PrintError("EH_CreateIndex: Cannot allocate info block\n");
            BF_CloseFile(fileDesc);
            return -1;
        }

    //initialize header info at block 0
    if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
        BF_PrintError("EH_CreateIndex: Cannot read block for header info\n");
        BF_CloseFile(fileDesc);
        return -1;
    }

    EH_info* fileInfo = newEH_info(2, fileDesc, attrType, attrName, attrLength, depth);
    printEH_info(fileInfo);
    memcpy(block, fileInfo, sizeof (EH_info));

    if (BF_WriteBlock(fileDesc, 0) < 0) {
        BF_PrintError("EH_CreateIndex: Cannot write to block for header info\n");
        BF_CloseFile(fileDesc);
        return -1;
    }

    // Create new address table: blocks 1 to addr_size

    if (EH_NewAddressTable(fileInfo)<0){
        printf("EH_CreateIndex: Could not create new address table\n");
        return -1;
    }

    // allocate blocks for 2^depth buckets

    for (i = 0; i < pow(2, depth); i++) {
        if (EH_NewDataBlock(fileDesc, depth) == -1) {
            printf("EH_CreateIndex: Could not create DataBlock!\n");
            return -1;
        }
    }

    if (BF_CloseFile(fileDesc) < 0) {
        BF_PrintError("EH_CreateIndex: Cannot close BF file\n");
        return -1;
    }
    return 0;
}

EH_info* EH_OpenIndex(char *fileName) {
    /* Add your code here */

    BF_Init();
    int fileDesc;
    if ((fileDesc = BF_OpenFile(fileName)) < 0) {
        BF_PrintError("EH_OpenIndex: Cannot open BF file\n");
        return NULL;

    }
    void* block;
    if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
        BF_PrintError("EH_OpenIndex: Cannot read block\n");
        BF_CloseFile(fileDesc);
        return NULL;
    }
    EH_info* fileInfo = malloc(sizeof (EH_info));
    memcpy(fileInfo, block, sizeof (EH_info));
    if (fileInfo->type != 2) {
        printf("EH_OpenIndex: Not a EH file!\n");
        BF_CloseFile(fileDesc);
        return NULL;
    }

    fileInfo->fileDesc = fileDesc;
    printf("fileDesc:%d for file %s\n", fileInfo->fileDesc, fileName);
    return fileInfo;

}

int EH_CloseIndex(EH_info* header_info) {
    /* Add your code here */

    int fileDesc = header_info->fileDesc;
    void* block;
    if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
        BF_PrintError("EH_CloseIndex: Cannot read block\n");
        BF_CloseFile(fileDesc);
        return -1;
    }
    header_info->fileDesc = -1;
    memcpy(block, header_info, sizeof (EH_info));

    if (BF_WriteBlock(fileDesc, 0) < 0) {
        BF_PrintError("EH_CloseIndex: Cannot write to block\n");
        BF_CloseFile(fileDesc);
        return -1;
    }

    if (BF_CloseFile(fileDesc) < 0) {
        BF_PrintError("EH_CloseIndex: Cannot close BF file\n");
        return -1;
    }
    deleteEH_info(header_info);
    return 0;
  
}

int EH_InsertEntry(EH_info* header_info, Record record) {
    /* Add your code here */

    printf("\nInsertEntry\n");
    printRecord(record);
    printf("Global Depth: %d\n", header_info->depth);


    //create hash for record
    unsigned int hash = 0;
    char binaryHash[33];
    int decimalHash;
    if (strcmp(header_info->attrName, "id") == 0) {
        hash = fnv_hash(&(record.id), 10);
    } else if (strcmp(header_info->attrName, "name") == 0) {
        hash = fnv_hash(&(record.name), 15);
    } else if (strcmp(header_info->attrName, "surname") == 0) {
        hash = fnv_hash(&(record.surname), 20);
    } else if (strcmp(header_info->attrName, "city") == 0) {
        hash = fnv_hash(&(record.city), 25);
    }

    to_binary(hash, binaryHash);
    to_decimal(binaryHash, &decimalHash, header_info->depth);
    printf("fnv_hash: %d\n",hash);
    printf("Hash: %s\n", binaryHash);
    printf("decimal hash: %d\n", decimalHash);


    // get address table
    int current_buckets = (int) pow(2, header_info->depth);
    int *addressTable = malloc( current_buckets * sizeof(int));
    if (EH_GetAddrTable(header_info, addressTable) == -1) {
        printf("EH_InsertEntry: Error Getting Address Table\n");
        return -1;
    }

    //find bucket using address table and hash
    int bucket = addressTable[decimalHash];



    //get bucket info
    DataBlock_info block_info;
    if (EH_GetDataBlockInfo(header_info->fileDesc, &block_info, bucket) == -1) {
        printf("EH_InsertEntry: Error Getting DataBlock Info\n");
        return -1;
    };


    //if block is full
    if (block_info.entriesNum == EH_SLOTS_FOR_RECORDS) {
        printf("EH_InsertEntry: Bucket Full!\n");
        //if bucket depth == total depth
        if (block_info.localDepth == header_info->depth) {
            printf("LocalDepth==Depth\n");
            EH_DoubleHashTable(header_info);
            return EH_InsertEntry(header_info,record);


        }
    }//if bucket depth < total depth

    else if (block_info.localDepth < header_info->depth) {


            EH_CreateNewBucket(header_info, decimalHash);
            return EH_InsertEntry(header_info, record);


        }

    else {
        //if block not full just insert
        printf("Record has to go to block no: %d \n",bucket);
        if (EH_InsertToDataBlock(header_info->fileDesc, bucket, record) == -1) {
            printf("EH_InsertEntry: Error Inserting To DataBlock");
            return -1;
        };
        //block_info.entriesNum++;
        //EH_UpdateDataBlockInfo(header_info->fileDesc,block_info,bucket);
    }
    free(addressTable);
    return 0;
}

int EH_GetAllEntries(EH_info *header_info, void *value) {
    /* Add your code here */
    printf("\nSearching for value: %s\n",(char*)value);
    unsigned int hash = 0;
    char binaryHash[33];
    int decimalHash;

    if (strcmp(header_info->attrName, "id") == 0) {
        hash = fnv_hash(value, 10);
    } else if (strcmp(header_info->attrName, "name") == 0) {
        hash = fnv_hash(value, 15);
    } else if (strcmp(header_info->attrName, "surname") == 0) {
        hash = fnv_hash(value, 20);
    } else if (strcmp(header_info->attrName, "city") == 0) {
        hash = fnv_hash(value, 25);
    }

    to_binary(hash, binaryHash);
    to_decimal(binaryHash, &decimalHash, header_info->depth);
    printf("fnv_hash: %d\n",hash);
    printf("Hash: %s\n", binaryHash);
    printf("decimal hash: %d\n", decimalHash);

    int current_buckets = (int) pow(2, header_info->depth);
    int *addressTable = malloc( current_buckets * sizeof(int));
    if (EH_GetAddrTable(header_info, addressTable) == -1) {
        printf("Error Getting Address Table\n");
        return -1;
    }

    //find bucket using address table and hash
    int bucket = addressTable[decimalHash];


    if( value != NULL ) {

        void* block;
        if(BF_ReadBlock( header_info->fileDesc, bucket, &block )< 0) {

            BF_PrintError( "HT_GetAllEntries: Error in BF_ReadBlock" );
            return -1;
        }

        int matched=0;
        Record record;
        DataBlock_info data_info;
        memcpy(&data_info,block, sizeof(DataBlock_info));

        int j;
        for(j=0; j<data_info.entriesNum; j++ ) {

            memcpy(&record, (block+ sizeof(DataBlock_info)+j*sizeof(Record) ), sizeof(Record) );

            if( !strcmp(header_info->attrName, "id") ){

                if( record.id == *(int *)value) {
                    printRecord( record );
                    matched ++;
                }

            } else if ( !strcmp(header_info->attrName, "name") ) {

                if( !strcmp( record.name, (char *)value ) ) {
                    printRecord(record);
                    matched ++;
                }

            } else if ( !strcmp(header_info->attrName, "surname") ) {

                if( !strcmp( record.surname, (char *)value ) ) {
                    printRecord( record );
                    matched ++;
                }

            } else if ( !strcmp(header_info->attrName, "city") ) {

                if( !strcmp( record.city, (char *)value ) ) {
                    printRecord( record );
                    matched ++;
                }

            }else {

                printf("HT_InsertEntry: Wrong attrName!\n");
                return -1;
            }

        }

    free(addressTable);
    printf("Matched %d records \n\n", matched);
    return 0;

    }
    else{

        /// print everything
        return 0;

    }



}

int EH_HashStatistics(char* filename) {


    EH_info* header;
    header=EH_OpenIndex(filename);

    //get address table

    int *addressTable = malloc( (int)pow(2,header->depth) * sizeof(int));
    if (EH_GetAddrTable(header, addressTable) == -1) {
        printf("Error Getting Address Table\n");
        return -1;
    }

    // total number of blocks in file

    int totalBlockNumber;
    if ((totalBlockNumber = BF_GetBlockCounter(header->fileDesc)) == -1) {
        BF_PrintError("block num ht get all");
        return -1;
    }
    printf("The total number of blocks of file %s is : %d\n",  filename, totalBlockNumber);

    void* block;
    int min_records=EH_SLOTS_FOR_RECORDS,max_records=0,mean_records=0;
    DataBlock_info B_info;

    int k;
    for (k=0; k<pow(2,header->depth); k++){

        BF_ReadBlock(header->fileDesc,addressTable[k],&block);
       // EH_PrintBlockOfRecords(block);
        EH_GetDataBlockInfo(header->fileDesc, &B_info,addressTable[k]);

        printf("Bucket no %d in block no: %d has %d records\n",k+1, addressTable[k],B_info.entriesNum);
        if(B_info.entriesNum==-1){
            printf("Error getting info for bucket no %d\n",k+1);
            return -1;
        }
        mean_records= mean_records+B_info.entriesNum;

        if(B_info.entriesNum>max_records){
            max_records=B_info.entriesNum;
        }
        if(B_info.entriesNum<min_records){
            min_records=B_info.entriesNum;
        }

    }

    mean_records=(int)(mean_records/pow(2,header->depth));
    printf("Minimum number of records in a bucket: %d\n",min_records);
    printf("Maximum number of records in a bucket: %d\n",max_records);
    printf("Mean number of records in a bucket: %d\n",mean_records);
    if (EH_CloseIndex( header) < 0) {
        printf("Error closing file\n");
        return -1;
    }

    return 0;
}
