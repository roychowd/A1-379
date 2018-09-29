all : a1jobs a1mon
a1jobs:
	gcc -Wall -g -std=c99 -ggdb -o a1jobs a1jobs.c

a1mon: 
	g++ -Wall a1mon.cc -o a1mon
clean:
	rm a1mon
	rm a1jobs
	rm -rf a1jobs.dSYM
	
