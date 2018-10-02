#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MAXLINE 5000

static int arrayIndex =0;
struct childInfo
{
    char *command;
    char* pid;
    char* ppid;
};

// typedef vector< tuple<string, string, string> 
void err_sys(const char *x);
void setLimit();
void displayInformation(int counter, char *targetPID, int seconds);
void getChilds(char *targetPID, struct childInfo **childarray);
bool monitorProcess(char * targetPID);
void terminateALL(struct childInfo **childarray);
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
    
    struct childInfo **childProcessArray =  calloc(32, sizeof(struct childInfo *));
    setLimit();
    getChilds(targetPID,childProcessArray);
    for (;;)
    {
        displayInformation(counter, targetPID, seconds);
        // getChilds(targetPID,childProcessArray);
        printf("List of Monitored Processes");
        int x = 0;
        printf("[");
        while (childProcessArray[x]!=NULL) {
            printf("%d:[%s,%s], " , x, childProcessArray[x]->pid, childProcessArray[x]->command);
            x++;
        } 
        printf("]\n");
        bool isKilled = monitorProcess(targetPID);
        if (isKilled) {
            terminateALL(childProcessArray);
        }
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

void getChilds(char *targetPID, struct childInfo **child )
{
    char line[MAXLINE];
    char psCommand[MAXLINE];
    strcpy(psCommand, "ps -o ppid,pid,cmd --ppid ");
    strcat(psCommand, targetPID);

    FILE *children;
    // if (arrayIndex > )

    if ((children = popen(psCommand, "r")) == NULL)
    {
        err_sys("popen error");
    }
    int counter = 0;
    while (fgets(line, sizeof(line), children))
    {
        if (counter >= 1) {
            char * token = strtok(line, " \n");
            if (strcmp(token, targetPID) == 0) {
                child[arrayIndex] = malloc(sizeof(struct childInfo));
                child[arrayIndex]->command = malloc(500 * sizeof(char));
                child[arrayIndex]->pid = malloc(500 * sizeof(char));
                child[arrayIndex]->ppid = malloc(500 * sizeof(char));
                char *childPID = strtok(NULL, " \n");
                // child[arrayIndex]->pid = childPID;
                char *cmd = strtok(NULL, " \n");
                strcpy(child[arrayIndex]->command, cmd);
                strcpy(child[arrayIndex]->pid, childPID);
                strcpy(child[arrayIndex]->ppid, token);
                arrayIndex++;
                getChilds(childPID, child);
            }
        }
        counter++;
    }
    if (pclose(children) < 0)
    {
        err_sys("pclose error");
    }
    
    
}

bool monitorProcess(char *targetPID) {
    char command[MAXLINE];
    char line[MAXLINE];
    int x = 0;
    strcpy(command, "ps ");
    strcat(command, targetPID);
    FILE *statusps;
    if ((statusps = popen(command, "r")) == NULL)
    {
        err_sys("popen error");
    }
    while (fgets(line, sizeof(line), statusps)) {
        x++;
    }
    if (pclose(statusps) < 0 ) {
        err_sys("pclose error");
    }

    if (x > 1) {
        return false;
    }
    else {
        return true;
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
    // childtuple t1; 
    printf("a1mon [counter= %d, pid= %d, target_pid= %s, interval= %d sec]\n", counter, getpid(), targetPID, seconds);
    if ((fpInput = popen("ps -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r")) == NULL)
    {
        err_sys("popen error");
    }
   
    while (fgets(line, MAXLINE, fpInput))
    {
        printf("%s", line);
    }
    if (pclose(fpInput) < 0)
    {
        err_sys("pclose error");
    }

    return;
}


void terminateALL(struct childInfo ** childarray) {
    printf("\na1mon appears to have terminated, cleaning up\n");
    int x = 0;
    while (childarray[x] != NULL) {
        pid_t pid = atoi(childarray[x]->pid);
        printf("terminating [%s, %s]\n", childarray[x]->pid, childarray[x]->command);
        kill(pid, SIGKILL);
        free(childarray[x]->command);
        free(childarray[x]->pid);
        free(childarray[x]->ppid);
        free(childarray[x]);
        x++;
    }
    printf("exiting a1mon\n");
    free(childarray);
    exit(1);
}