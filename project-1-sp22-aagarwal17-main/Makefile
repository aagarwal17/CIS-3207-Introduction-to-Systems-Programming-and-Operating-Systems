# -*- MakeFile -*-

#target: dependencies
#	action

all: tups
CC = g++
WFLAGS = -Wall -Werror
GDB = -std=c++11 -ggdb

tups: src/*
#	gcc src/* -o tups -Wall -Werror
	$(CC) $(GDB) src/* -o tups $(WFLAGS)

# parser.o: parser.c
# 	gcc -c parser.c

# optionProcessor.o: optionProcessor.c
# 	gcc -c optionProcessor.c

# getProcessList.o: getProcessList.c
# 	gcc -c getProcessList.c

# main.o: main.c
# 	gcc -c main.c

clean:
	rm -f *.o tups		