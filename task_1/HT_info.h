

#ifndef ERGASIA001_HT_INFO_H_
#define ERGASIA001_HT_INFO_H_


/* Όπου attrType, attrName, και attrLength αφορούν το πεδίο κλειδί, fileDesc είναι ο αναγνωριστικός
αριθμός του ανοίγματος του αρχείου, όπως επιστράφηκε από το επίπεδο διαχείρισης μπλοκ, και
numBuckets είναι το πλήθος των κάδων που υπάρχουν στο αρχείο. */
typedef struct {
    int type;                                                          /* ο τύπος του αρχείου */
    int fileDesc;           /* αναγνωριστικός αριθμός ανοίγματος αρχείου από το επίπεδο block */
    char attrType;/* ο τύπος του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο, 'c' ή'i' */
    char* attrName;        /* το όνομα του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο */
    int attrLength;      /* το μέγεθος του πεδίου που είναι κλειδί για το συγκεκριμένο αρχείο */
    int numBuckets;                 /* το πλήθος των “κάδων” του αρχείου κατακερματισμού */
    int addr_size;                       /* το μέγεθος του πίνακα διευθύνσεων σε αριθμό μπλοκ */
}HT_info;

HT_info* newHT_info( int type, int fileDesc, char attrType, char* attrName, int attrLength, int numBuckets );

void printHT_info(HT_info* FileInfo);
void deleteHT_info(HT_info* FileInfo);

#endif //ERGASIA001_HT_INFO_H
