//
// Created by katerina on 17/1/2017.
//

#include "ES_info.h"


#include <stdio.h>
#include <stdlib.h>

ES_info* newES_info(int type, int fileDesc, int fieldNo)
{
    ES_info* fileInfo = (ES_info *) malloc(sizeof (ES_info));
    fileInfo->type = type;
    fileInfo->fileDesc = fileDesc;
    fileInfo->fieldNo = fieldNo;
    return fileInfo;
}

void printES_info(ES_info* FileInfo){
    printf( "Type of file: %d\nFileDesc: %d\nfieldNo: %d\n",
            FileInfo->type, FileInfo->fileDesc, FileInfo->fieldNo);
}

void deleteES_info(ES_info* FileInfo)
{
    free(FileInfo);
}
