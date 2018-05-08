#include "HT_info.h"
#include "BF.h"
#include "DataFunctions.h"



HT_info* newHT_info(int type, int fileDesc, char attrType, char* attrName, int attrLength, int numBuckets){
        HT_info* fileInfo = (HT_info *) malloc(sizeof (HT_info));
        fileInfo->type = type;
        fileInfo->fileDesc = fileDesc;
        fileInfo->attrType = attrType;
        fileInfo->attrLength = attrLength;
        fileInfo->numBuckets = numBuckets;
        int slots=(int)((BLOCK_SIZE-(sizeof (AddrTable_info)))/(sizeof(AddressEntry)));
        int size;
        if(numBuckets%slots!=0)size=(numBuckets/slots)+1;
        else size=(numBuckets/slots);
        fileInfo->addr_size = size;
        fileInfo->attrName = attrName;
        printf("New HT_info created: %s\n",fileInfo->attrName);
        return fileInfo;
    }

void printHT_info(HT_info* FileInfo){
        printf( "Type of file: %d\nFileDesc: %d\nattrType: %c\n"
                "attrName: %s\nattrLength: %d\nNumber of buckets: %d\n"
                        "size of address table: %d\n",
                FileInfo->type, FileInfo->fileDesc, FileInfo->attrType,
                FileInfo->attrName, FileInfo->attrLength, FileInfo->numBuckets,
                FileInfo->addr_size);
    }

void deleteHT_info(HT_info* FileInfo){
        free(FileInfo);
    }


