
OBJS1	= hash.o HT_info.o hashfunc.o DataFunctions.o Record.o main_example_HT.o
OBJS2	= exhash.o EH_info.o exhashfunc.o ExDataFunctions.o Record.o main_example_EH.o
OUT1	= execHT
OUT2	= execEH
CC	= gcc
FLAGS	= -c -Wall
LIB 	= libBF_32.a -lm


# OBJECTS

# STATIC
hash.o: hash.c hash.h
	$(CC) $(FLAGS) hash.h hash.c

HT_info.o: HT_info.h HT_info.c
	$(CC) $(FLAGS) HT_info.h HT_info.c

hashfunc.o: hashfunc.h hashfunc.c
	$(CC) $(FLAGS) hashfunc.h hashfunc.c

DataFunctions.o: DataFunctions.h DataFunctions.c
	$(CC) $(FLAGS) DataFunctions.h DataFunctions.c



# EXTENDIBLE
exhash.o: exhash.c exhash.h
	$(CC) $(FLAGS) exhash.h exhash.c

EH_info.o: EH_info.h EH_info.c
	$(CC) $(FLAGS) EH_info.h EH_info.c

exhashfunc.o: exhashfunc.h exhashfunc.c
	$(CC) $(FLAGS) exhashfunc.h exhashfunc.c

ExDataFunctions.o: ExDataFunctions.h ExDataFunctions.c
	$(CC) $(FLAGS) ExDataFunctions.h ExDataFunctions.c

#COMMON
Record.o: Record.h Record.c
	$(CC) $(FLAGS) Record.h Record.c


# test HT

main_example_HT.o: main_example_HT.c
	$(CC) $(FLAGS) main_example_HT.c
t1: $(OBJS1)
	$(CC) $(OBJS1) $(LIB) -o $(OUT1)

# test EH

main_example_EH.o: main_example_EH.c
	$(CC) $(FLAGS) main_example_EH.c
t2: $(OBJS2)
	$(CC) $(OBJS2) $(LIB) -o $(OUT2)

# clean up
clean:
	rm -f $(OBJS1) $(OUT1)
	rm -f $(OBJS2) $(OUT2)) $(OUT2)
