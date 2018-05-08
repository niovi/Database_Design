#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "Sorted.h"
#include "BF.h"
#include "DataFunctions.h"

#define fileName "heap"

void insert_Entries(int filedesc) {
    //TO DO
    //read from input stream and write on fd

    FILE *stream;
    printf("insert_Entries\n");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    stream = fopen("./datasets/10000.csv","r");
    printf("csv open: %s\n", (char *) stream);
    Record record;



    while ((read = getline(&line, &len, stream)) != -1) {
        line[read - 2] = 0;
        char *pch;

        pch = strtok(line, ",");
        record.id = atoi(pch);

        pch = strtok(NULL, ",");
        pch++;
        pch[strlen(pch) - 1] = 0;
        strncpy(record.name, pch, sizeof(record.name));

        pch = strtok(NULL, ",");
        pch++;
        pch[strlen(pch) - 1] = 0;
        strncpy(record.surname, pch, sizeof(record.surname));

        pch = strtok(NULL, ",");
        pch++;
        pch[strlen(pch) - 1] = 0;
        strncpy(record.city, pch, sizeof(record.city));



        assert(!Sorted_InsertEntry(filedesc, record));
    }
    free(line);
    fclose(stream);
    printf("csv closed\n");
}


int main(int argc, char **argv) {

    int fd;
    int fieldNo=3;

    BF_Init();

    //create heap file
    if(Sorted_CreateFile(fileName) == -1  )
        printf("Error creating file!\n");

    fd = ES_OpenFile(fileName);
    if( fd == -1  )
        printf("Error opening file!\n");
    insert_Entries(fd);

    Sorted_CloseFile(fd);


    Sorted_checkSortedFile(fileName,fieldNo);

    //sort heap file using 2-way merge-sort
    if(Sorted_SortFile(fileName,fieldNo) == -1  )
        printf("Error sorting file!\n");

    //printf("Sorting completed, back in main\n");

    char name[20];
    strcpy(name,"heapSorted");
    char str[10];
    snprintf(str, 10, "%d", fieldNo);
    strcpy(name, strcat(name, str));
    //printf("name is: %s\n", name1);

    if(Sorted_checkSortedFile(name,fieldNo) == -1  )printf("Error sorting file!\n");


    Sorted_checkSortedFile(name,3);
    //get all entries with value
    char value[20];
    strcpy(value, "Keratsini");
    //fieldNo = 0;
    //int value = 11903588;

    fd = Sorted_OpenFile(name);
    if( fd == -1  )
        printf("Error opening file!\n");

   Sorted_GetAllEntries(fd,fieldNo,&value);




    return EXIT_SUCCESS;
}
