# -*- MakeFile -*-

#target: dependencies
#	action

all: tups

tups: parser.o optionProcessor.o getProcessList.o main.o
	gcc parser.o optionProcessor.o getProcessList.o main.o -o tups -Wall -Werror

parser.o: parser.c
	gcc -c parser.c

optionProcessor.o: optionProcessor.c
	gcc -c optionProcessor.c

getProcessList.o: getProcessList.c
	gcc -c getProcessList.c

main.o: main.c
	gcc -c main.c

clean:
	rm -f *.o tups		