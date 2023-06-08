all: main.o functions.o header.o registers.o utils.o funcoesFornecidas.o tree.o search.o
	gcc -o programaTrab *.o -g

run:
	./programaTrab

clean:
	rm *.o programaTrab
	clear

main.o: main.c functions.h header.h registers.h utils.h funcoesFornecidas.h tree.h
	gcc -c main.c -g

functions.o: functions.c functions.h
	gcc -c functions.c -g

header.o: header.c header.h 
	gcc -c header.c -g

registers.o: registers.c registers.h 
	gcc -c registers.c -g

utils.o: utils.c utils.h
	gcc -c utils.c -g

funcoesFornecidas.o: funcoesFornecidas.c funcoesFornecidas.h
	gcc -c funcoesFornecidas.c -g

tree.o: tree.c tree.h
	gcc -c tree.c -g

search.o: search.c search.h
	gcc -c search.c -g