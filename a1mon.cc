#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <string>
#include <map>
extern "C"
{
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
}
using namespace std;
#define MAXLINE 5000

struct childInfo
{
    char *command;
    int pid;
    int ppid;
};

// typedef vector< tuple<string, string, string> 


void err_sys(const char *x);
void setLimit();
void displayInformation(int counter, char *targetPID, int seconds);
void getChilds(char *targetPID, char *line);

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
    struct childInfo **childProcessArray = (struct childInfo **) calloc(32, sizeof(struct childInfo *));

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
    for (;;)
    {
        displayInformation(counter, targetPID, seconds);
     
        counter++;
        // std::cout << "List of monitored processes:" << endl;
        // std::cout << "[0:[]]" << endl;
        sleep(seconds);
    }

    return 0;
}

void err_sys(const char *x)
{
    perror(x);
    exit(1);
}

void getChilds(char *targetPID, char * line)
{
    
    std::cout << "CALLED GETCHILDS";
    return;
    // char line[MAXLINE];
    // char psCommand[MAXLINE];
    // strcpy(psCommand, "ps -o pid,cmd,ppid --ppid ");
    // strcat(psCommand, targetPID);
    // int index = 0;

    // FILE *children;

    // if ((children = popen(psCommand, "r")) == NULL)
    // {
    //     err_sys("popen error");
    // }
    // while (fgets(line, sizeof(line), children))
    // {
    //     if (strstr(line, targetPID))
    //     {
    //         // char *oneArg = strdup(line);
    //         // strtok(oneArg, " ");
    //         std::string aString = std::string(line);
    //         std::cout << aString;
    //         char * token = strtok(line, " ");
    //         while (token != NULL ) {
    //             std::cout << token << endl;
    //             token=strtok(NULL, " "); 
    //         }
    //     }
    // }
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
    // childtuple t1; 
    printf("a1mon [counter= %d, pid= %d, target_pid= %s, interval= %d sec]\n", counter, getpid(), targetPID, seconds);
    if ((fpInput = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL)
    {
        err_sys("popen error");
    }
    while (fgets(line, MAXLINE, fpInput))
    {

        
        std::string lineString(line);
        char *p = targetPID;
        // std::cout << lineString;
        char *token = strtok(line, " \n");
        int position = 0;
        while (token != NULL) {
            // parent process
            if ((position == 1)) {
                std::cout << token << endl;
                
            }
            // std::cout << token << endl;
            position++;
            token = strtok(NULL, " \n");
      
        
        }


        // char *token = strtok(line, " ");
        // while (token != NULL) {
        //     // std::cout << token << endl;
        //     pidPosition++;
        //     token = strtok(NULL, " ");
        //     if (pidPosition == 1) {
        //         if (strcmp(targetPID, token) == 0) {
        //             std::cout << token << endl;

        //         }
                
        //     }
        // }
        
    }
    if (pclose(fpInput) < 0)
    {
        err_sys("pclose error");
    }

    return;
}
