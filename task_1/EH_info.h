

#ifndef INFO_H
#define INFO_H

/* H δομή όπου κρατούνται όσες πληροφορίες κρίνετε απαραίτητες προκειμένου να μπορείτε να
επεξεργαστείτε στη συνέχεια τις εγγραφές του. Tα attrName, attrType και attrLength αφορούν το
πεδίο-κλειδί, fileDesc είναι ο αναγνωριστικός αριθμός του ανοίγματος του αρχείου, όπως
επιστράφηκε από το επίπεδο διαχείρισης μπλοκ και depth είναι το ολικό βάθος του ευρετηρίου
επεκτατού κατακερματισμού. */
typedef struct {
    int type;                                                         /* ο τύπος του αρχείου */
    int fileDesc;          /* αναγνωριστικός αριθμός ανοίγματος αρχείου από το επίπεδο block */
    char* attrName;    /* το όνομα του πεδίου που είναι το κλειδί για το συγκεκριμένο αρχείο */
    char attrType;                                        /* τύπος πεδίου-κλειδιού: 'c', 'i' */
    int attrLength;                                                 /* μήκος πεδίου-κλειδιού */
    int depth;                        /* το ολικό βάθος ευρετηρίου επεκτατού κατακερματισμού */
    int addr_size;                       /* το μέγεθος του πίνακα διευθύνσεων σε αριθμό μπλοκ */
    int offset;
} EH_info;

EH_info* newEH_info( int type, int fileDesc, char attrType, char* attrName, int attrLength, int depth );

void printEH_info(EH_info* FileInfo);
void deleteEH_info(EH_info* FileInfo);


#endif //ERGASIA001_EH_INFO_H
