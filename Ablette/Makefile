Ablette: Ablette.o ../libs/IPC/Partage_memoire.o 
	gcc -o Ablette Ablette.o ../libs/IPC/Partage_memoire.o -lrt -g -Wall -lpcap

Ablette.o: Ablette.c
	gcc -c Ablette.c -o Ablette.o -lrt -g -Wall -lpcap

clean:
	rm -f Ablette *.o
