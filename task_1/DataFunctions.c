

#include "DataFunctions.h"

// get info for each bucket

Bucket_info HT_GetBucketInfo(int fileDesc,int bucket_num){


    DataBlock_info Iblock;
    Bucket_info Ibucket,Ibucket_next;
    Ibucket.records=Ibucket.blocks=-1;
    if(HT_GetDataBlockInfo(fileDesc,&Iblock,bucket_num)<0){
        printf("Error getting block info for no %d\n",bucket_num);
        return Ibucket ;}
    if(Iblock.overflow==0){
        Ibucket.blocks=1;
        Ibucket.records=Iblock.entriesNum;
    }
    else{
        Ibucket_next=HT_GetBucketInfo(fileDesc,Iblock.overflow);
        Ibucket.blocks=1+Ibucket_next.blocks;
        Ibucket.records=Iblock.entriesNum+Ibucket_next.records;
    }

    //printf("HT_GetBucketInfo: Success!\n");
    return Ibucket;
}

//Create and initialize new Address Table Block

int HT_NewAddrTableBlock(int fileDesc, int buckets) {
    //printf("HT_NewAddrTableBlock: start\n");
    void *block;
    int buckets_to_write=buckets;
    AddressEntry *addressTable = (AddressEntry*)malloc(buckets * sizeof(AddressEntry));
    //printf("HT_NewAddrTableBlock: malloc addressTable\n");
    //int slots=(int)((BLOCK_SIZE-(sizeof (AddrTable_info)))/(sizeof(AddressEntry)));
    //printf("HT_NewAddrTableBlock: slots pes block available %d\n", SLOTS_FOR_BUCKETS);
    int size;
    if(buckets%HT_SLOTS_FOR_BUCKETS!=0)size=(buckets/HT_SLOTS_FOR_BUCKETS)+1;
    else size=(buckets/HT_SLOTS_FOR_BUCKETS);

    int i;
    for (i = 0; i < buckets; ++i) {
        addressTable[i].first_block = 1+size+i;
        addressTable[i].last_block = 1+size+i;
        //printf("bucket no: %d first: %d last: %d\n", i+1,addressTable[i].first_block,addressTable[i].last_block);
        // 0 for index, 1 to size for address table, size+1 to end for data blocks
    }
    //printf("HT_NewAddrTableBlock: addressTable is set\n");
    //HT_PrintAddressTable(addressTable,buckets);

    int k;
    for (k=0; k<size; k++){


        //printf("HT_NewAddrTableBlock: creating block no %d\n",k+1);
        //printf("HT_NewAddrTableBlock: buckets to put in table %d\n",buckets_to_write);
        AddrTable_info block_info;
        if(k==size-1)block_info.overflow=0;
        else block_info.overflow = k+2;

        if (BF_AllocateBlock(fileDesc) < 0) {
            BF_PrintError(" HT_NewAddrTableBlock: Cannot allocate block!\n");
            return -1;
        }

        if (BF_ReadBlock(fileDesc, k+1, &block) < 0) {
            BF_PrintError(" HT_NewAddrTableBlock: Cannot read from block!\n");
            return -1;
        }

        memcpy(block, &block_info, sizeof (AddrTable_info));
        if(buckets_to_write>HT_SLOTS_FOR_BUCKETS){
        memcpy(block+(sizeof (AddrTable_info)), addressTable+(k*HT_SLOTS_FOR_BUCKETS), HT_SLOTS_FOR_BUCKETS * sizeof(AddressEntry));
            //printf("addressTable+(k*slots) = %d\n", addressTable+(k*SLOTS_FOR_BUCKETS));
            //printf("addressTable+((k+1)*slots) = %d\n", addressTable+((k+1)*SLOTS_FOR_BUCKETS));
            //printf("addressTable[k*slots + 1] = %d\n", addressTable[k*SLOTS_FOR_BUCKETS+1]);
        buckets_to_write=buckets_to_write-HT_SLOTS_FOR_BUCKETS;

            if (BF_WriteBlock(fileDesc, k+1) < 0) {
                BF_PrintError(" HT_NewAddrTableBlock: Cannot write to block!\n");
                BF_CloseFile(fileDesc);
                return -1;
            }
        //HT_PrintBlockOfInts(block);
        }

        else{
            memcpy(block+(sizeof (AddrTable_info)), &addressTable[k*HT_SLOTS_FOR_BUCKETS], buckets_to_write * sizeof(AddressEntry));
            buckets_to_write=0;
            //printf("HT_NewAddrTableBlock: last block written\n");

            if (BF_WriteBlock(fileDesc, k+1) < 0) {
                BF_PrintError(" HT_NewAddrTableBlock: Cannot write to block!\n");
                BF_CloseFile(fileDesc);
                return -1;
            }
        //HT_PrintBlockOfInts(block);
        }
    }
    //printf("HT_NewAddrTableBlock: end\n");
    printf("HT_NewAddrTableBlock: Success!\n");
    return 0;
}

//Create and initialize new Data Block

int HT_NewDataBlock(int fileDesc, int blockNumber) {
    if (BF_AllocateBlock(fileDesc) < 0) {
        BF_PrintError(" HT_NewDataBlock: Cannot allocate new data block\n");
        return -1;
    }



    void* block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError(" HT_NewDataBlock: Cannot read new data block\n");
        return -1;
    }

    DataBlock_info block_info;
    block_info.entriesNum = 0;
    block_info.overflow = 0;
    memcpy(block, &block_info, sizeof (DataBlock_info));

    if (BF_WriteBlock(fileDesc, blockNumber) < 0) {
        BF_PrintError(" HT_NewDataBlock: Cannot write to new data block\n");
        return -1;
    }
    //printf("HT_NewDataBlock: Success!\n");
    return 0;
}

//Copy Data Block info to block info pointer

int HT_GetDataBlockInfo(int fileDesc, DataBlock_info *block_info, int blockNumber) {
    void *block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError(" HT_GetDataBlockInfo: Cannot read data block\n");
        return -1;
    }
    memcpy(block_info, block, sizeof (DataBlock_info));
    //printf("HT_GetDataBlockInfo: Success!\n");
    return 0;
}

//Update Data Block info with block_info

int HT_UpdateDataBlockInfo(int fileDesc, DataBlock_info block_info, int blockNumber) {
    void* block;
    if (BF_ReadBlock(fileDesc, blockNumber, &block) < 0) {
        BF_PrintError(" HT_UpdateDataBlockInfo: Cannot read data block\n");
        return -1;
    }
    memcpy(block, &block_info, sizeof (DataBlock_info));
    if (BF_WriteBlock(fileDesc, blockNumber) < 0) {
        BF_PrintError(" HT_UpdateDataBlockInfo:Cannot write to data block\n");
        return -1;
    }
    //printf("HT_UpdateDataBlockInfo: Success!\n");
    return 0;
}

// Insert record to data block

int HT_InsertToDataBlock(HT_info info,int bucket, int blockNumber, Record record) {
    void* block;

    DataBlock_info block_info;
    if (HT_GetDataBlockInfo(info.fileDesc, &block_info, blockNumber) < 0) {
        BF_PrintError(" HT_InsertToDataBlock: Cannot get data block info\n");
        return -1;
    }

    //int slots=(BLOCK_SIZE-sizeof(DataBlock_info)) / sizeof (Record);
    //printf("block number: %d\n",blockNumber);
    //printf("overflow: %d  num of records: %d slots: %d\n",block_info.overflow,block_info.entriesNum, slots);
/*
    if (block_info.entriesNum == SLOTS_FOR_RECORDS && block_info.overflow != 0) {
        //if block is full and there is an overflow block
        //insert to that block
      //  printf("Going to overflow bucket\n");
         return HT_InsertToDataBlock(info, bucket,block_info.overflow, record);

    }

*/

    //else
    if (block_info.entriesNum < HT_SLOTS_FOR_RECORDS && block_info.overflow == 0) {
        //if there is still space in the block
        //copy record to block
        if (BF_ReadBlock(info.fileDesc, blockNumber, &block) < 0) {
            BF_PrintError(" HT_InsertToDataBlock: Cannot read data block\n");
            return -1;
        }
        memcpy(block + sizeof (DataBlock_info)+(block_info.entriesNum * sizeof (Record)), &record, sizeof (Record));
        block_info.entriesNum = block_info.entriesNum + 1;
        memcpy(block, &block_info, sizeof (DataBlock_info));
        if (BF_WriteBlock(info.fileDesc, blockNumber) < 0) {
            BF_PrintError(" HT_InsertToDataBlock: Cannot write to data block\n");
            return -1;
        }
        printf("Record put in block no %d\n",blockNumber);
        printRecord(record);
        //printf("HT_InsertToDataBlock: Success!\n");
        return 1;
    }



    else if (block_info.entriesNum == HT_SLOTS_FOR_RECORDS && block_info.overflow == 0) {
        // if there is no space left in block and there is no overflow block
        //create a new block
        //printf("No overflow block!\n");


        int totalBlockNum;
        if ((totalBlockNum = BF_GetBlockCounter(info.fileDesc)) == -1) {
            BF_PrintError(" HT_InsertToDataBlock: BF_GetBlockCounter error\n");
            return -1;
        }

        //overflow pointer now points to the new block
        block_info.overflow = totalBlockNum;
        if (HT_UpdateDataBlockInfo(info.fileDesc, block_info, blockNumber) < 0) {
            printf("Error Updating Data Block Info");
            return -1;
        }

        if (HT_NewDataBlock(info.fileDesc, block_info.overflow) < 0) {
            BF_PrintError("HT_NewDataBlock");
            return -1;
        }
        //printf("New overflow created\n");
        //insert to overflow block
        HT_UpdateAddressTable(info,bucket,block_info.overflow);
       return HT_InsertToDataBlock(info,bucket, block_info.overflow, record);
    }




    else{
        //printf("overflow: %d  num of records: %d\n",block_info.overflow,block_info.entriesNum);
    //printf("HT_InsertToDataBlock: reached end -> ERROR\n");
    return -1;}
}


//copy the address table from block number 1 to addressTable pointer

int HT_GetAddrTable(int fileDesc, AddressEntry *addressTable) {
    //printf("Getting address table..,\n");
    HT_info info;
    int entries;
    void* block;
    if (BF_ReadBlock(fileDesc, 0, &block) < 0) {
        BF_PrintError("HT_GetAddrTable: BF_ReadBlock error\n");
        return -1;
    }

    memcpy(&info, block,  sizeof (HT_info));
    //printHT_info(&info);
    //int slots=(int)((BLOCK_SIZE-(sizeof (AddrTable_info)))/(sizeof(AddressEntry)));
    entries = info.numBuckets;
    int k;
    for(k=0;k<info.addr_size;k++){
        //printf("reading block no: %d\n",k+1);
        if (BF_ReadBlock(fileDesc, k+1 , &block) < 0) {
            BF_PrintError("HT_GetAddrTable: BF_ReadBlock error\n");
            return -1;
        }
        if (entries>HT_SLOTS_FOR_BUCKETS)
        {
            //printf("entries left %d\n",entries);
            memcpy(&addressTable[k*HT_SLOTS_FOR_BUCKETS], block+sizeof(AddrTable_info),  HT_SLOTS_FOR_BUCKETS*sizeof (AddressEntry));
         entries = entries - HT_SLOTS_FOR_BUCKETS;}
        else {memcpy(&addressTable[k*HT_SLOTS_FOR_BUCKETS], block+sizeof(AddrTable_info),  entries*sizeof (AddressEntry));
                entries=0;}

    }

    /*  // Print address table
    int n;
    for(n=0;n<info.numBuckets;n++){
        printf("bucket no %d: first block: %d last block: %d\n",n+1,addressTable[n].first_block,addressTable[n].last_block);
    }
    */
    //HT_PrintAddressTable(addressTable,info.numBuckets);
    //printf("HT_GetAddrTable: Success!\n");
    return 0;
}

// update the precise block with address change for specific bucket

int HT_UpdateAddressTable(HT_info header,int bucket,int last_block){
    int k=0;
    void* block;
    //printf("Updating Address Table!\n");
    int where_is_the_bucket = bucket;
    while(where_is_the_bucket>HT_SLOTS_FOR_BUCKETS){
        where_is_the_bucket = where_is_the_bucket - HT_SLOTS_FOR_BUCKETS;
        k++;}
    if (BF_ReadBlock(header.fileDesc, k+1 , &block) < 0) {
        BF_PrintError("HT_UpdateAddressTable: BF_ReadBlock error\n");
        return -1;
    }
    memcpy(block+sizeof(AddrTable_info)+(where_is_the_bucket-1)*sizeof(AddressEntry)+sizeof(int),&last_block,sizeof(int));
    if (BF_WriteBlock(header.fileDesc, k+1) < 0) {
        BF_PrintError(" HT_InsertToDataBlock: Cannot write to data block\n");
        return -1;
    }
    //printf("HT_UpdateAddressTable: Success!\n");
    return 0;
}

// print whole block as series of ints

void HT_PrintBlockOfInts(void *block){

    int *block1 = block;
    int num_of_ints = BLOCK_SIZE/sizeof(int);
    int n;
    for (n=0;n<num_of_ints;n++){
        printf("%d\t",block1[n]);
    }
    printf("\n");
}