all: compile execute

compile: projet.o
	gcc -Wall projet.o -o exec -lncurses

projet.o: projet.c
	gcc -Wall projet.c -c -lncurses

execute: exec game1.txt game2.txt game3.txt game4.txt
	./exec

clean:
	rm *.o exec

install:
	sudo apt-get install make gcc libncurses5-dev
