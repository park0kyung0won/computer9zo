simulator: main.o simul.o fileread.o datatypes.o cycleindex.o
	gcc -o simulator main.o simul.o fileread.o datatypes.o cycleindex.o

main.o: main.c
	gcc -c -v main.c

simul.o: simul.c
	gcc -c -v simul.c

fileread.o: fileread.c
	gcc -c -v fileread.c

datatypes.o: datatypes.c
	gcc -c -v datatypes.c

cycleindex.o: cycleindex.c
	gcc -c -v cycleindex.c

all: simulator
