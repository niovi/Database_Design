
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "BF.h"
#include "Record.h"
#include "ExDataFunctions.h"
#include "exhash.h"


int EH_CreateNewBucket(EH_info *info, int decimal_hash){

    //get address Table
    int *addressTable = malloc(((int)(pow(2, info->depth)) * sizeof (int)));
    if (EH_GetAddrTable(info, addressTable) == -1) {
        printf("EH_CreateNewBucket: Error Getting Address Table\n");
        return -1;
    }

    // use temporary block to rearrange buckets
    DataBlock_info block_info1;
    void *block_temp = malloc(BLOCK_SIZE);
    void *block1 = malloc(BLOCK_SIZE);

    int bucket = addressTable[decimal_hash];
    if (EH_GetDataBlockInfo(info->fileDesc, &block_info1, bucket) == -1) {
        printf("Error Getting DataBlock Info");
        return -1;
    };
    if (BF_ReadBlock(info->fileDesc, bucket, &block1) < 0) {
        BF_PrintError("EH_OpenIndex: Cannot read block\n");
        BF_CloseFile(info->fileDesc);
        return -1;
    }

    memset(block_temp,0,BLOCK_SIZE);
    memcpy(block_temp,block1,BLOCK_SIZE);
    memset(block1,0,BLOCK_SIZE);

    block_info1.localDepth++;
    block_info1.entriesNum=0;

    memcpy(block1,&block_info1, sizeof(DataBlock_info));

    if (BF_WriteBlock(info->fileDesc, bucket) < 0) {
        BF_PrintError("EH_CloseIndex: Cannot write to block\n");
        BF_CloseFile(info->fileDesc);
        return -1;
    }

    if (EH_NewDataBlock(info->fileDesc,block_info1.localDepth)<0){
        printf("EH_CreateNewBucket: Error creating new data block\n");
    }

    int total_block_number=0;
    if ((total_block_number = BF_GetBlockCounter(info->fileDesc)) < 0) {
        BF_PrintError("EH_NewDataBlock: BF_GetBlockCounter error\n");
        return -1;
    }

    int count = 0;
    int from=0;
    int to;
    int i;
    for (i = 0; i < pow(2, info->depth); i++) {
        if (addressTable[i] == bucket) {
            if (count == 0)
                from = i;
            count++;
        }
    }
    from = from + (count / 2);
    to = from + count - 1;

    int w;
    for(w=from; w<to;w++){
        addressTable[w] = total_block_number-1;
    }


    // Save address table
    EH_UpdateAddrTable(*info,addressTable);

    // rearrange records
    Record record;
    int j;
    for(j=0; j<EH_SLOTS_FOR_RECORDS; j++ ) {

        memcpy(&record, (block_temp + sizeof(DataBlock_info) + j * sizeof(Record)), sizeof(Record));
        EH_InsertEntry(info,record);

    }

    //free(block1);
    //free(block_temp);
    return 0;
}

int EH_NewAddressTable(EH_info* header_info) {

    printf("EH_NewAddressTable: Creating new address table...\n");
    void *block;
    int i;
    int buckets_to_write = (int) pow(2, header_info->depth);


    int *addressTable = malloc( buckets_to_write * sizeof(int));

    for (i = 0; i < buckets_to_write; i++) {
        addressTable[i] = i + 1 + header_info->addr_size;
        //printf("bucket no %d to block no%d memory address %d\n", i + 1, addressTable[i], &addressTable[i]);
    }
    //printf("EH_NewAddressTable: Address table set...\n");
    //EH_PrintBlockOfInts(addressTable);
    //initialize address table at blocks 1 to addr_size
    for (i = 0; i < header_info->addr_size; i++) {

        //printf("EH_NewAddressTable: creating block no %d\n", i + 1);
        //printf("EH_NewAddressTable: buckets to put in table %d\n", buckets_to_write);
        AddrTable_info block_info;
        if (i == header_info->addr_size - 1)block_info.overflow = 0;
        else block_info.overflow = i + 2;

        if (BF_AllocateBlock(header_info->fileDesc) < 0) {
            BF_PrintError("EH_NewAddressTable: Cannot allocate info block\n");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
        //printf("EH_NewAddressTable: NEW block no %d\n", i + 1);
        if (BF_ReadBlock(header_info->fileDesc, i+1, &block) < 0) {
            BF_PrintError("EH_NewAddressTable: BF_ReadBlock error \n");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
        memcpy(block, &block_info, sizeof(int));
        //EH_PrintBlockOfInts(block);

        if (buckets_to_write > EH_SLOTS_FOR_BUCKETS) {
            memcpy(block + (sizeof(AddrTable_info)), &addressTable[i * EH_SLOTS_FOR_BUCKETS], EH_SLOTS_FOR_BUCKETS*
                                                                                              sizeof(int));
            //printf("Entries of buckets per block of address table: %d\n",   EH_SLOTS_FOR_BUCKETS);
            //printf("Copying from: %d to %d\n", addressTable + (i * EH_SLOTS_FOR_BUCKETS),addressTable + (i * EH_SLOTS_FOR_BUCKETS)+ EH_SLOTS_FOR_BUCKETS  );
            //printf("Copying values from: %d to %d\n", addressTable[i * EH_SLOTS_FOR_BUCKETS],addressTable[i * EH_SLOTS_FOR_BUCKETS+ EH_SLOTS_FOR_BUCKETS]  );
            //printf("addressTable+(k*slots) = %d\n", addressTable+(i*EH_SLOTS_FOR_BUCKETS));
            //printf("addressTable+((k+1)*slots) = %d\n", addressTable+((i+1)*EH_SLOTS_FOR_BUCKETS));
            //printf("addressTable[k*slots + 1] = %d\n", addressTable[i*EH_SLOTS_FOR_BUCKETS+1]);
            buckets_to_write = buckets_to_write - EH_SLOTS_FOR_BUCKETS;

            if (BF_WriteBlock(header_info->fileDesc, i + 1) < 0) {
                BF_PrintError("EH_NewAddressTable: Cannot write to block!\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }
            //EH_PrintBlockOfInts(block);
        } else {
            memcpy(block + (sizeof(AddrTable_info)), &addressTable[i * EH_SLOTS_FOR_BUCKETS], buckets_to_write* sizeof(int));
            buckets_to_write = 0;
            //printf("EH_NewAddressTable: last block written\n");

            if (BF_WriteBlock(header_info->fileDesc, i + 1) < 0) {
                BF_PrintError("EH_NewAddressTable: Cannot write to block!\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }
            //EH_PrintBlockOfInts(block);

        }

    //EH_PrintBlockOfInts(block);
    }

        free(addressTable);
        //printf("EH_NewAddressTable: Address table ready!\n");
        return 0;

    }

int EH_DoubleHashTable(EH_info* header_info){

    void* block;
    int blockNumTotal = 0;
    AddrTable_info addr_info;
    int oldSize = (int) pow(2, header_info->depth);
    int newSize = (int) pow(2, header_info->depth+1);
    printf("Old size: %d New size: %d\n", oldSize, newSize);

    //get address Table
    int *addressTable = malloc(oldSize* sizeof (int));
    if (EH_GetAddrTable(header_info, addressTable) == -1) {
        printf("EH_DoubleHashTable: Error Getting Address Table 01\n");
        return -1;
    }

    // duplicate address table
    int *NEWaddressTable = malloc(newSize * sizeof (int));
    int i;
    for (i = 0; i < oldSize; i++) {
        NEWaddressTable[2 * i] = addressTable[i];
        NEWaddressTable[(2 * i) + 1] = addressTable[i];
    }

    // save new address table
    int buckets_to_write = newSize;

    // in previously allocated blocks
    int k;
    for(k=0; k< header_info->addr_size;k++){

        if (BF_ReadBlock(header_info->fileDesc, k+1, &block) < 0) {
            BF_PrintError("EH_DoubleHashTable: BF_ReadBlock error 02\n");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
        memcpy(&addr_info, block, sizeof(AddrTable_info));

        if(buckets_to_write>EH_SLOTS_FOR_BUCKETS) {
            int l;
            for (l = 0; l < EH_SLOTS_FOR_BUCKETS; l++) {
                memcpy(block + sizeof(AddrTable_info) + l * sizeof(int), &NEWaddressTable[l + k * EH_SLOTS_FOR_BUCKETS],
                       sizeof(int));
            }
            buckets_to_write = buckets_to_write - EH_SLOTS_FOR_BUCKETS;
        }
        else{

            int l;
            for (l = 0; l < buckets_to_write; l++) {
                memcpy(block + sizeof(AddrTable_info) + l * sizeof(int), &NEWaddressTable[l + k * EH_SLOTS_FOR_BUCKETS],
                       sizeof(int));
            }
            buckets_to_write = 0;
        }
        if (BF_WriteBlock(header_info->fileDesc, k + 1) < 0) {
            BF_PrintError("EH_DoubleHashTable: Cannot write to block! 03\n");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        if(addr_info.overflow==0 && buckets_to_write>0){

            if (BF_AllocateBlock(header_info->fileDesc) < 0) {
                BF_PrintError("EH_DoubleHashTable: Cannot allocate info block 04\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }

            if ((blockNumTotal = BF_GetBlockCounter(header_info->fileDesc)) < 0) {
                BF_PrintError("EH_DoubleHashTable: BF_GetBlockCounter error 05\n");
                return -1;
            }
            addr_info.overflow=blockNumTotal-1;
            if (BF_ReadBlock(header_info->fileDesc, k+1, &block) < 0) {
                BF_PrintError("EH_DoubleHashTable: BF_ReadBlock error 06\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }
            memcpy(block,&addr_info, sizeof(AddrTable_info));
            if (BF_WriteBlock(header_info->fileDesc, k + 1) < 0) {
                BF_PrintError("EH_DoubleHashTable: Cannot write to block! 07\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }
        }

    }

    // if there is need to allocate new blocks for address table
    while(buckets_to_write>0){

        if(buckets_to_write>EH_SLOTS_FOR_BUCKETS){
            int current_block = addr_info.overflow;
            if (BF_ReadBlock(header_info->fileDesc, current_block , &block) < 0) {
                BF_PrintError("EH_DoubleHashTable: BF_ReadBlock error 08\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }

            int l;
            for(l=0; l<EH_SLOTS_FOR_BUCKETS; l++){
                memcpy(block+sizeof(AddrTable_info)+l*sizeof(int),&NEWaddressTable[l+k*EH_SLOTS_FOR_BUCKETS], sizeof(int));
            }
            k++;
            buckets_to_write = buckets_to_write - EH_SLOTS_FOR_BUCKETS;
            if(buckets_to_write>0){

                if (BF_AllocateBlock(header_info->fileDesc) < 0) {
                    BF_PrintError("EH_DoubleHashTable: Cannot allocate info block 09\n");
                    BF_CloseFile(header_info->fileDesc);
                    return -1;
                }


                if ((blockNumTotal = BF_GetBlockCounter(header_info->fileDesc)) < 0) {
                    BF_PrintError("EH_DoubleHashTable: BF_GetBlockCounter error 10\n");
                    return -1;
                }
                addr_info.overflow=blockNumTotal-1;

            } else addr_info.overflow=0;

            memcpy(block,&addr_info, sizeof(AddrTable_info));
            if (BF_WriteBlock(header_info->fileDesc, current_block) < 0) {
                BF_PrintError("EH_DoubleHashTable: Cannot write to block! 11\n");
                BF_CloseFile(header_info->fileDesc);
                return -1;
            }
            }

        else{
            int current_block = addr_info.overflow;
        if (BF_ReadBlock(header_info->fileDesc, current_block, &block) < 0) {
            BF_PrintError("EH_DoubleHashTable: BF_ReadBlock error 12\n");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }
        memcpy(&addr_info, block, sizeof(AddrTable_info));
        int l;
        for(l=0; l<buckets_to_write; l++){
            memcpy(block+sizeof(AddrTable_info)+l*sizeof(int),&NEWaddressTable[l+k*EH_SLOTS_FOR_BUCKETS], sizeof(int));
        }

            buckets_to_write=0;

        addr_info.overflow=0;

        memcpy(block,&addr_info, sizeof(AddrTable_info));
        if (BF_WriteBlock(header_info->fileDesc,current_block) < 0) {
            BF_PrintError("EH_DoubleHashTable: Cannot write to block! 13\n");
            BF_CloseFile(header_info->fileDesc);
            return -1;
        }

        }

    }
    ////////     Update header info    ///////////
    header_info->depth++;
    int buckets = (int) pow(2,header_info->depth);
    if (buckets%EH_SLOTS_FOR_BUCKETS==0)
        header_info->addr_size = buckets/EH_SLOTS_FOR_BUCKETS;
    else header_info->addr_size = buckets/EH_SLOTS_FOR_BUCKETS + 1;

    if (BF_ReadBlock(header_info->fileDesc, 0, &block) < 0) {
        BF_PrintError("EH_DoubleHashTable: Cannot read block 14\n");
        BF_CloseFile(header_info->fileDesc);
        return -1;
    }

    memcpy(block, header_info, sizeof (EH_info));

    if (BF_WriteBlock(header_info->fileDesc, 0) < 0) {
        BF_PrintError("EH_DoubleHashTable: Cannot write to block 15\n");
        BF_CloseFile(header_info->fileDesc);
        return -1;
    }

    printf("EH_DoubleHashTable: Address table updated with new depth \n");
    //free(block);
    //free(NEWaddressTable);
    //free(addressTable);
    return 0;

}

int EH_NewDataBlock(int fileDesc, int localDepth) {
    //printf("EH_NewDataBlock\n");
    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError("EH_NewDataBlock: Cannot allocate block\n");
        return -1;
    }

    int blockNumTotal = 0;
    if ((blockNumTotal = BF_GetBlockCounter(fileDesc)) < 0) {
        BF_PrintError("EH_NewDataBlock: BF_GetBlockCounter error\n");
        return -1;
    }

    void* block;
    if (BF_ReadBlock(fileDesc, blockNumTotal - 1, &block) < 0) {
        BF_PrintError("EH_NewDataBlock: Cannot read block\n");
        return -1;
    }

    DataBlock_info block_info;
    block_info.entriesNum = 0;
    block_info.localDepth = localDepth;
    //block_info.overflow = 0;
    memcpy(block, &block_info, sizeof (DataBlock_info));

    if (BF_WriteBlock(fileDesc, blockNumTotal - 1) < 0) {
        BF_PrintError("EH_NewDataBlock: Cannot write to block\n");
        return -1;
    }
    //EH_PrintBlockOfInts(block);
    return 0;
}

int EH_GetDataBlockInfo(int fileDesc, DataBlock_info *block_info, int blockNumber) {
    void *block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError("EH_GetDataBlockInfo: Cannot read block\n");
        return -1;
    }
    memcpy(block_info, block, sizeof (DataBlock_info));
    return 0;
}

int EH_InsertToDataBlock(int fileDesc, int blockNumber, Record record) {
    void* block;
    DataBlock_info block_info;

    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError("BF_ReadBlock");
        return -1;
    }
    //EH_PrintBlockOfRecords(block);

    memcpy(&block_info,block, sizeof(DataBlock_info));
    printf("Data block info: local depth: %d  num of entries: %d \n",block_info.localDepth,block_info.entriesNum);

    memcpy(block + sizeof (DataBlock_info)+(block_info.entriesNum * sizeof (Record)), &record, sizeof (Record));
    block_info.entriesNum ++;
    memcpy(block, &block_info, sizeof (DataBlock_info));

    if (BF_WriteBlock(fileDesc, blockNumber) < 0) {
        BF_PrintError("BF_WriteBlock");
        return -1;
    }

    printf("Record put in block: %d\n",blockNumber);
    printf("Current entries:%d in block: %d with local depth %d\n\n",block_info.entriesNum,blockNumber,block_info.localDepth);

    return 0;
}

int EH_GetAddrTable(EH_info *info , int *addressTable) {
    void* block;
    int entries = (int)pow(2,info->depth);
    AddrTable_info addr_header;
    int k;
    int j=1;
    for(k=0;k<info->addr_size;k++){
        //printf("reading block no: %d\n",k+1);
        if (BF_ReadBlock(info->fileDesc, j , &block) < 0) {
            BF_PrintError("EH_GetAddrTable: BF_ReadBlock error\n");
            return -1;
        }
        if (entries>EH_SLOTS_FOR_BUCKETS)
        {
            //printf("entries left %d\n",entries);
            memcpy(&addr_header,block, sizeof(AddrTable_info));
            j=addr_header.overflow;

            memcpy(&addressTable[k*EH_SLOTS_FOR_BUCKETS], block+sizeof(AddrTable_info),  EH_SLOTS_FOR_BUCKETS*sizeof (int));
            entries = entries - EH_SLOTS_FOR_BUCKETS;
        }
        else {
            memcpy(&addressTable[k*EH_SLOTS_FOR_BUCKETS], block+sizeof(AddrTable_info),  entries*sizeof (int));
            entries=0;
        }

    }

      // Print address table
/*
    int n;
    for(n=0;n<pow(2,info->depth);n++){
        printf("bucket no %d:  block: %d\n",n+1,addressTable[n]);
    }
*/
    return 0;
}

int EH_UpdateAddrTable(EH_info header, int *addressTable) {

    AddrTable_info addr_info;
    addr_info.overflow=100;
    int bucket_entries = (int)pow(2,header.depth);
    int k=0; // up to size of address table -1
    int i=1; // next block to read
    while(addr_info.overflow!=0){

        void* block;
        if (BF_ReadBlock(header.fileDesc, i, &block) < 0) {
            BF_PrintError("EH_UpdateAddrTable: BF_ReadBlock\n");
            return -1;
        }

        memcpy(&addr_info,block, sizeof(AddrTable_info));
        printf("AddrTable info: %d overflow block\n",addr_info.overflow);
        if(bucket_entries>EH_SLOTS_FOR_BUCKETS){
            memcpy(block+ sizeof(AddrTable_info), addressTable+k*sizeof(int), EH_SLOTS_FOR_BUCKETS * sizeof (int));
            bucket_entries = bucket_entries - EH_SLOTS_FOR_BUCKETS;
        }
        else{
            memcpy(block+ sizeof(AddrTable_info), addressTable+k*sizeof(int), bucket_entries * sizeof (int));

        }
        if (BF_WriteBlock(header.fileDesc, i) < 0) {
            BF_PrintError("EH_UpdateAddrTable: BF_WriteBlock\n");
            return -1;
        }
        i=addr_info.overflow;
        k++;
    }

    return 0;
}

void EH_PrintBlockOfInts(void *block){

        int *block1 = block;
        int num_of_ints = BLOCK_SIZE/sizeof(int);
        //printf("num 0f ints in this block: %d\nsize of int: %d\n",num_of_ints, sizeof(int));
        int n;
        for (n=0;n<num_of_ints;n++){
            printf("%d\t",block1[n]);
        }
        printf("\n");
    }

void EH_PrintBlockOfRecords(void *block){

    DataBlock_info data_header;
    memcpy(&data_header,block, sizeof(DataBlock_info));
    Record record;
    printf("num of records in this block: %d local depth: %d \n",data_header.entriesNum, data_header.localDepth);
    int n;
    for (n=0;n<data_header.entriesNum;n++){
        memcpy(&record,(block+ sizeof(DataBlock_info)+n* sizeof(Record)), sizeof(Record));
        printRecord(record);
    }
    printf("\n");
}