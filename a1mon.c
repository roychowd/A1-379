#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MAXLINE 5000

void err_sys(const char *x);
void setLimit();
void displayInformation(int counter, char *targetPID, int seconds);
void getChilds(char *targetPID);

int main(int argc, char const *argv[])
{
    /* code */
    /** 
    *  program takes an argument of target_pid - mandatory 
    *  program takes an optional arugment of interval
    */
    int counter = 0;
    int seconds;
    char *targetPID;

    if (argc < 2 || argc > 3)
    {
        err_sys("Incorrect number of arguments");
    }
    targetPID = (char *)argv[1];

    if (argc == 3)
    {
        seconds = atoi(argv[2]);
    }
    else
    {
        seconds = 3;
    }
    setLimit();

    // children_map = grabProcessChildren(targetPID);
    getChilds(targetPID);
    // for (;;)
    // {
    //     displayInformation(counter, targetPID, seconds);

    //     counter++;
    //     // std::cout << "List of monitored processes:" << endl;
    //     // std::cout << "[0:[]]" << endl;
    //     sleep(seconds);
    // }

    return 0;
}

void err_sys(const char *x)
{
    perror(x);
    exit(1);
}

void getChilds(char *targetPID)
{
    char line[MAXLINE];
    char psCommand[MAXLINE];
    strcpy(psCommand, "ps -o pid,cmd,ppid --ppid ");
    strcat(psCommand, targetPID);

    FILE *children;

    if ((children = popen(psCommand, "r")) == NULL)
    {
        err_sys("popen error");
    }
    while (fgets(line, sizeof(line), children))
    {
        
	printf("%s", line);
	if (strstr(line, targetPID))
        {
            printf("the line with the ppid child is %s", line);
        }
    }
}

void setLimit()
{
    // sets the cpu limit to 10 minutes
    // put this in a header file to share with my other a1jobs
    const struct rlimit maxLimit = {600, 600};
    if (setrlimit(RLIMIT_CPU, &maxLimit) < 0)
    {
        err_sys("set limit error");
    }
}

void displayInformation(int counter, char *targetPID, int seconds)
{
    char line[MAXLINE];
    FILE *fpInput;
    printf("a1mon [counter= %d, pid= %d, target_pid= %s, interval= %d sec]\n", counter, getpid(), targetPID, seconds);
    if ((fpInput = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL)
    {
        err_sys("popen error");
    }
    while (fgets(line, MAXLINE, fpInput))
    {

        if (fputs(line, stdout) == EOF)
        {
            err_sys("error in fputs");
        }
    }
    if (pclose(fpInput) < 0)
    {
        err_sys("pclose error");
    }

    return;
}
