//
// Created by katerina on 17/1/2017.
//

#include "Record.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Record* newRecord(int id, char *name[15], char surname[20], char city[25]) {
    Record* record = (Record *) malloc(sizeof (Record));
    record->id = id;
    strcpy(record->name, *name);
    strcpy(record->surname, surname);
    strcpy(record->city, city);
    return record;
}

void printRecord(Record record) {
    printf("Id: %d\nName: %s\nSurname: %s\nCity: %s\n\n"
            , record.id, record.name, record.surname,record.city);
}

void deleteRecord(Record *record) {
    free(record);
    printf("Record deleted");
}
