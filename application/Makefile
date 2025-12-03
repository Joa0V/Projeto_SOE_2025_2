all: main.o gps.o frame.o
	gcc -o ../main main.o gps.o frame.o

main.o: main.c gps.h frame.h
	gcc -c main.c

gps.o: gps.h gps.c
	gcc -c gps.c

frame.o: frame.h frame.c
	gcc -c frame.c

clean:
	rm -f *.o main
