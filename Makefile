all : a1jobs a1mon
a1jobs:
	gcc -Wall -g -std=c99 -ggdb -o a1jobs a1jobs.c

a1mon: 
	gcc -Wall -g -std=c99 -ggdb  a1mon.c -o a1mon

tar:
	tar -cvf submit.tar Makefile a1jobs.c a1mon.c Assignment\ 1\ -\ Project\ Report.pdf

clean:
	rm a1jobs
	rm a1mon
	
