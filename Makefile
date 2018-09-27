a1jobs:
	gcc -Wall -g -std=c99 -ggdb -o a1jobs a1jobs.c

clean:
	rm a1jobs
	rm -rf a1jobs.dSYM
