# all : a1jobs a1mon
# a1jobs:
	# gcc -Wall -g -std=c99 -ggdb -o a1jobs a1jobs.c

a1mon: 
	gcc -Wall -g -std=c99 -ggdb  a1mon.c -o a1mon
clean:
	rm a1mon
	rm -rf a1jobs.dSYM
	
