//
// Created by katerina on 17/1/2017.
//

#ifndef EXTERNALSORT_ES_INFO_H
#define EXTERNALSORT_ES_INFO_H


/* H δομή όπου κρατούνται όσες πληροφορίες κρίνετε απαραίτητες προκειμένου να μπορείτε να
επεξεργαστείτε στη συνέχεια τις εγγραφές του. Tα attrName, attrType και attrLength αφορούν το
πεδίο-κλειδί, fileDesc είναι ο αναγνωριστικός αριθμός του ανοίγματος του αρχείου, όπως
επιστράφηκε από το επίπεδο διαχείρισης μπλοκ και depth είναι το ολικό βάθος του ευρετηρίου
επεκτατού κατακερματισμού. */
typedef struct {
    int type;                                                         /* ο τύπος του αρχείου */
    int fileDesc;          /* αναγνωριστικός αριθμός ανοίγματος αρχείου από το επίπεδο block */
    int fieldNo;
} ES_info;

ES_info* newES_info( int type, int fileDesc, int fieldNo);

void printES_info(ES_info* FileInfo);
void deleteES_info(ES_info* FileInfo);


#endif //EXTERNALSORT_ES_INFO_H
