sioux: sioux.o ../libs/Reseau/server.o analyse_http.o multi_threads.o
	gcc -o sioux sioux.o ../libs/Reseau/server.o  analyse_http.o multi_threads.o ../libs/IPC/Partage_memoire.o -lrt

sioux.o: sioux.c
	gcc -c sioux.c -o sioux.o -lrt

analyse_http.o: analyse_http.c
	gcc -c analyse_http.c -o analyse_http.o

multi_threads.o: multi_threads.c
	gcc -c multi_threads.c -o multi_threads.o
clean:
	rm -f sioux *.o *.csv Ablette.html
