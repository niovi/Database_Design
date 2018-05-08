//
// Created by katerina on 17/1/2017.
//

#ifndef EXTERNALSORT_RECORD_H
#define EXTERNALSORT_RECORD_H


typedef struct {
    int id;
    char name[15];
    char surname[20];
    char city[25];

}Record;


Record* newRecord(int id, char *name[15], char surname[20], char city[25]);


void printRecord(Record record);
void deleteRecord(Record *record);



#endif //EXTERNALSORT_RECORD_H
