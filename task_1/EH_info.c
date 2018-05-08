#include "EH_info.h"
#include "BF.h"
#include "ExDataFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

EH_info* newEH_info(int type, int fileDesc, char attrType, char* attrName, int attrLength, int depth)
{
    EH_info* fileInfo = (EH_info *) malloc(sizeof (EH_info));
    fileInfo->type = type;
    fileInfo->fileDesc = fileDesc;
    fileInfo->attrType = attrType;
    fileInfo->attrLength = attrLength;
    fileInfo->depth = depth;
    int buckets = (int) pow(2,depth);
    if (buckets%EH_SLOTS_FOR_BUCKETS==0)
    fileInfo->addr_size = buckets/EH_SLOTS_FOR_BUCKETS;
    else fileInfo->addr_size = buckets/EH_SLOTS_FOR_BUCKETS + 1;
    fileInfo->attrName=attrName;
    fileInfo->offset = fileInfo->addr_size;
    printf("New HT_info created: %s\n",fileInfo->attrName);
    return fileInfo;
}

void printEH_info(EH_info* FileInfo){
    printf( "Type of file: %d\nFileDesc: %d\nattrType: %c\n"
                    "attrName: %s\nattrLength: %d\nMax Depth: %d\nAddress size in blocks: %d\n",
            FileInfo->type, FileInfo->fileDesc, FileInfo->attrType,
            FileInfo->attrName, FileInfo->attrLength, FileInfo->depth, FileInfo->addr_size );
}

void deleteEH_info(EH_info* FileInfo)
{
    free(FileInfo);
}