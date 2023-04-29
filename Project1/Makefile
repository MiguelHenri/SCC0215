all: main.o functions.o header.o registers.o utils.o funcoesFornecidas.o
	gcc -o programaTrab *.o

run:
	./programaTrab

clean:
	rm *.o programaTrab
	clear

main.o: main.c functions.h header.h registers.h utils.h funcoesFornecidas.h
	gcc -c main.c 

functions.o: functions.c functions.h
	gcc -c functions.c

header.o: header.c header.h
	gcc -c header.c

registers.o: registers.c registers.h
	gcc -c registers.c

utils.o: utils.c utils.h
	gcc -c utils.c

funcoesFornecidas.o: funcoesFornecidas.c funcoesFornecidas.h
	gcc -c funcoesFornecidas.c