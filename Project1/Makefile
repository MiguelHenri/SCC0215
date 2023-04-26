all: main.o functions.o header.o
	gcc -o programaTrab *.o

run:
	./programaTrab

clean:
	rm *.o programaTrab
	clear

main.o: main.c functions.h header.h
	gcc -c main.c 

functions.o: functions.c functions.h
	gcc -c functions.c

header.o: header.c header.h
	gcc -c header.c