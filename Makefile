all: gameoflife.o libgol.so
	gcc -Wall -Wextra -pedantic -std=c11 gameoflife.o -L. -lgol -o gameoflife

libgol.so: gol.o
	gcc -Wall -Wextra -pedantic -std=c11 -shared gol.o -o ./libgol.so

gol.o: gol.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -fPIC -c gol.c -o gol.o

gameoflife.o: gameoflife.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -c gameoflife.c -o gameoflife.o

clean:
	rm -rf gameoflife gameoflife.o libgol.so gol.o