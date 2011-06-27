all: heur

heur: main.o heuristica.o dados.o
	gcc -Wall main.o heuristica.o dados.o -o heur

main.o: main.c
	gcc -Wall -c main.c

heuristica.o: heuristica.c
	gcc -Wall -c heuristica.c

dados.o: dados.c
	gcc -Wall -c dados.c

clean:
	rm -rf *.o heur
