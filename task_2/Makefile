
OBJS1	= Sorted.o ES_info.o Record.o main.o DataFunctions.o
OUT1	= execES
CC	= gcc
FLAGS	= -c -Wall

UNAME := $(shell uname -p)
ifeq ($(UNAME),i686)
	LIB = libBF_32.a -lm
else
	LIB = libBF_64.a -lm
endif




# OBJECTS



# EXTERNAL SORT
Sorted.o: Sorted.c Sorted.h
	$(CC) $(FLAGS) Sorted.h Sorted.c

DataFunctions.o: DataFunctions.c DataFunctions.h
	$(CC) $(FLAGS) DataFunctions.h DataFunctions.c

ES_info.o: ES_info.c ES_info.h
	$(CC) $(FLAGS) ES_info.h ES_info.c 

Record.o: Record.h Record.c
	$(CC) $(FLAGS) Record.h Record.c


# test ES

main.o: main.c
	$(CC) $(FLAGS) main.c
t1: $(OBJS1)
	$(CC) $(OBJS1) $(LIB) -o $(OUT1)



# clean up
clean:
	rm -f $(OBJS1) $(OUT1)
