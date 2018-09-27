#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct jobInfo
{
    char *command;
    int index;
    int pid;
};

void err_sys(const char *x);
char **grabArgumentsWithoutRun(char *line);
void list_all_jobs();
void run_pgm(char **args, struct jobInfo *job);
void recordTimes()
{
    struct tms t;
    clock_t userCPUTime, systemCPUTime;
    //. Call function times() (see the table above) to record the user and CPU times for the current process
    //(and its terminated children).
    if (times(&t) == -1)
    {
        err_sys("times error");
    }
    printf(" times() yields: user CPU=%.2f; system CPU: %.2f\n", (double)t.tms_utime,
           (double)t.tms_stime);
    userCPUTime = t.tms_utime;
    systemCPUTime = t.tms_stime;
}

int main()
{
    // ANCHOR MAIN
    /* code */
    struct rlimit rlim;
    int parentPid;
    size_t buffer = 0;
    // char *line = malloc(length * sizeof(char));
    // char *copyString, *pch;
    char *line = NULL;
    char **arguments = NULL;

    /** initialize an array of struct pointers to jobs
     * each job is essentially represented as a  struct
     * so i need to malloc it as a struct
     */
    struct jobInfo **jobArray = malloc(32 * sizeof(struct jobInfo *));
    int jobIndex = 0;

    /* obtain current limit */
    if (getrlimit(RLIMIT_CPU, &rlim) < 0)
    {
        err_sys("getrlimit error");
    }

    /* set cpu limit of 10 minutes (1 * 60 * 10)  = 600 seconds */
    // need to learn how to implement this!
    rlim.rlim_cur = 600;
    if (setrlimit(RLIMIT_CPU, &rlim) < 0)
    {
        err_sys("setrlimit error");
    }

    // need to somehow record times
    // need to learn how to implement this
    recordTimes();
    // grab parent id
    parentPid = getpid();

    // start of main loop

    while (1)
    {
        //ANCHOR While
        printf("a1jobs[%d]: ", parentPid);
        // grabUserCMD(&length, line);
        getline(&line, &buffer, stdin);
        printf("%s", line);
        //printf("First argument: %s, second argument: %s", arguments[0], arguments[1]);
        int idx = 0;

        if (strcmp(line, "quit\n") == 0)
        {
            break;
        }
        if (strstr(line, "run") != NULL)
        {
            arguments = grabArgumentsWithoutRun(line);

            if (jobIndex < 32)
            {
                /**
                * parameters to send this function - will delete this later when i go back to refactor
                * nothing really all i need to do is return a pointer to a struct object allocted in memory!!!!!;
                *
                */

                //TODO put cmd line inside jobarray[jobindex]
                jobArray[jobIndex] = malloc(sizeof(struct jobInfo));
                jobArray[jobIndex]->index = jobIndex;
                printf("job array index %d", jobArray[jobIndex]->index);
                run_pgm(arguments, jobArray[jobIndex]);
                printf("job array pid of child %d\n", jobArray[jobIndex]->pid);
                jobIndex++;
            }
        }

        // else if (strcmp(arguments[0], "list") == 0)
        // {
        //     list_all_jobs();
        // }
    }

    free(line);
    free(jobArray);
    return 0;
}

/**
 * A function that eases repettive code for errors
 */
void err_sys(const char *x)
{
    perror(x);
    exit(1);
}

/**
 * Function that takes in the line and parses it to grab the arguments
 * returns an array of strings (char **)
 * i used this website to help implement it: 
 * 
 */
char **grabArgumentsWithoutRun(char *line)
{
    int buffersize = 64;
    int index = 0;
    char **args = malloc(buffersize * sizeof(char *));
    char *oneArg = strdup(line);

    if (!args) // if malloc fails
    {
        err_sys("Malloc error");
    }
    int firstelement = 0;
    oneArg = strtok(oneArg, " \n");
    while (oneArg != NULL)
    {
        if (firstelement != 0)
        {
            args[index] = oneArg;
            index++;
        }
        firstelement++;
        if (index > buffersize)
        {
            // reallocate array
            buffersize += buffersize;
            args = realloc(args, buffersize);
        }
        oneArg = strtok(NULL, " \n");
    }

    args[index] = NULL;

    return args;
}

/**
 * Function that executes if the person types a command that uses list
 */
void list_all_jobs()
{
    //ANCHOR LIST
    printf("LISTING ALL JOBS\n");
}

/**
 * function that executes when the user enters run pgm arg1 ... arg4
 * requries all the arguments
 */

void run_pgm(char **args, struct jobInfo *job)
{
    //ANCHOR Run Program
    printf("entering run prgm\n");
    pid_t pid;
    int ret = 1;
    int status = 0;
    int argIndex = 1;
    pid = fork();

    if (pid < 0)
    {
        printf("failed forked");
        err_sys("failed forked");
    }
    else if (pid == 0)
    {
        /* code */
        printf("%d", getpid());
        if (execvp(args[0], args) < 0)
        {
            err_sys("Execvp error");
        }
    }
    else
    {
        // fork returns the pid of the new child in the parents
        // so pid now holds the child pid in here
        // so i need to put htis inside a struct with an int representing the index
        // actually nvm i can implement that later
        job->pid = pid;
        printf("the pid of the child is %d\n", pid);
        // printf("parent process, pid = %u\n", getpid());
    }
}