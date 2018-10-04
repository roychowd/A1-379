// CREATED BY DHRUBA CHOWDHURY - CCID: KONISHKY - 1466117

#define _GNU_SOURCE

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>


// a struct representing a job
struct jobInfo
{
    char *command;
    int index;
    int pid;
    bool isKilled;
};


// functions that i need
void err_sys(const char *x);
char **grabArgumentsWithoutRun(char *line);
void list_all_jobs();
void run_pgm(char **args, struct jobInfo *job);
int grabIndex(char *line);
void suspendjob(struct jobInfo *job);
void resumejob(struct jobInfo *job);
void terminateJob(struct jobInfo *job);
void pr_times(clock_t realTime, struct tms *start, struct tms *end);
void setLimit();


// --------------------------------------------------MAIN PROGRAM -------------------------------------------- // 
int main()
{
    /* code */

    // following is from APUE [SR 3/E]
    struct tms tmsStart, tmsEnd;
    clock_t start, end;
    // End from APUE

    int parentPid;
    size_t buffer = 0;

    // initialize a char pointer and a array of pointers to strings
    char *line = NULL; // used in the getline function 
    char **arguments = NULL;

    /** initialize an array of struct pointers to jobs
     */

    // initialaize a jobarray thats a array of struct objects
    struct jobInfo **jobArray = calloc(32, sizeof(struct jobInfo *)); 
    int jobIndex = 0;

    setLimit();

    if ((start = times(&tmsStart)) == -1)
        err_sys("times error");

    // grab parent id
    parentPid = getpid();

    // start of main loop

    while (1)
    {
        // grab user function 
        printf("a1jobs[%d]: ", parentPid);
        getline(&line, &buffer, stdin);

        // if statement to compare user input with assignment specifications 
        if (strcmp(line, "quit\n") == 0)
        {
            break; // if quit we exit the main loop without terminating 
        }
        else if (strstr(line, "run") != NULL)
        {
            arguments = grabArgumentsWithoutRun(line); // functions that grabs arguments without the run command
            if (jobIndex < 32)
            {
                jobArray[jobIndex] = malloc(sizeof(struct jobInfo)); // allocate memory for a struct
                jobArray[jobIndex]->index = jobIndex;
                run_pgm(arguments, jobArray[jobIndex]); // runs the program
                jobIndex++;
            }
        }

        else if (strcmp(line, "list\n") == 0)
        {
            int i = 0;
            while (jobArray[i] != NULL)
            {
                printf("%d:  (pid=\t%d,  cmd=%s)\n", jobArray[i]->index, jobArray[i]->pid, jobArray[i]->command);
                i++;
            }
        }
        else if (strstr(line, "suspend"))
        {
            int suspendIndex = grabIndex(line); 
            if (jobArray[suspendIndex] != NULL)
            {

                suspendjob(jobArray[suspendIndex]); // suspends the job
            }
        }
        else if (strstr(line, "resume"))
        {
            int resumeIndex = grabIndex(line); // grabs the index
            if (jobArray[resumeIndex] != NULL)
            {
                resumejob(jobArray[resumeIndex]); // resumes the job 
            }
        }
        else if (strstr(line, "terminate"))
        {
            int terminateIndex = grabIndex(line);
            if (jobArray[terminateIndex] != NULL)
            {
                terminateJob(jobArray[terminateIndex]); // terminates the job
            }
        }
        else if (strcmp(line, "exit\n") == 0)
        {
            int k = 0;
            while ((jobArray[k] != NULL))
            {
                if (jobArray[k]->isKilled == false)
                {
                    terminateJob(jobArray[k]);
                }

                k++;
            }
            break;
        }
    }

    if ((end = times(&tmsEnd)) == -1)
    {
        err_sys("times error");
    }

    // implementing APUE code;
    pr_times(end - start, &tmsStart, &tmsEnd);


    // memory deallocation // 
    free(line); 
    free(arguments); 
    int i = 0;
    while (jobArray[i] != NULL)
    {
        free(jobArray[i]->command);
        free(jobArray[i]);
        i++;
    }
    free(jobArray);
    return 0;
}

// ------------------------------------------------END OF MAIN ---------------------------------------------//



/**
 * A function that is called when errors occur
 */
void err_sys(const char *x)
{
    perror(x);
    exit(1);
}

/**
 * Function that takes in the line with the cmd run and parses it to grab the arguments
 * returns an array of strings (char **)
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
 * The following function is from APUE 3/E fig. 8.31
 * Used to print times of processes 
 */
void pr_times(clock_t realTime, struct tms *start, struct tms *end)
{
    static long clockTick = 0;
    if (clockTick == 0)
    {
        // we will fetch clock ticks per second first time
        if ((clockTick = sysconf(_SC_CLK_TCK)) < 0)
        {

            err_sys("sysconf error");
        }
    }
    printf("real:\t%7.2f\n", realTime / (double)clockTick);
    printf("user:\t%7.2f\n", (end->tms_utime - start->tms_utime) / (double)clockTick);
    printf("sys:\t%7.2f\n", (end->tms_stime - start->tms_stime) / (double)clockTick);
    printf("child user:\t%7.2f\n", (end->tms_cutime - start->tms_cutime) / (double)clockTick);
    printf("child sys:\t%7.2f\n", (end->tms_cstime - start->tms_cstime) / (double)clockTick);
}



/**
 * Function that executes if the person types a command that uses list
 */
int grabIndex(char *line)
{
    int index;
    bool isIndex = true;
    line = strtok(line, " \n");
    char *s;
    while (isIndex)
    {
        if ((s = strtok(NULL, " \n")) != NULL)
        {
            index = atoi(s);
        }
        isIndex = false;
    }
    return index;
}


// function used to suspend jobs
void suspendjob(struct jobInfo *job)
{
    kill(job->pid, SIGSTOP);
    return;
}


// function used to resume jobs
void resumejob(struct jobInfo *job)
{
    kill(job->pid, SIGCONT);
    return;
}

// function used to resume jobs
void terminateJob(struct jobInfo *job)
{
    kill(job->pid, SIGKILL);
    job->isKilled = true;
    printf("\t\tjob %d terminated\n", job->pid);
}

/**
 * function that executes when the user enters run pgm arg1 ... arg4
 * requries all the arguments
 */
void run_pgm(char **args, struct jobInfo *job)
{
    //ANCHOR Run Program
    pid_t pid;
    pid = fork();

    if (pid < 0)
    {
        err_sys("failed forked");
    }
    else if (pid == 0)
    {
        /* code */
        if (execvp(args[0], args) < 0)
        {
            err_sys("Execvp error");
        }
    }
    else
    {
        // fork returns the pid of the new child in the parents
        // so pid now holds the child pid in here
        job->pid = pid;
        job->isKilled = false;
        job->command = malloc(500 * sizeof(char));
        int x = 1;
        strcpy(job->command, args[0]);
        while (args[x] != NULL)
        {
            strcat(job->command, " ");
            strcat(job->command, args[x]);
            x++;
        }
    }
}

// Function used from the APUE BOOK [S/E 3]
void setLimit()
{
    // sets the cpu limit to 10 minutes
    const struct rlimit maxLimit = {600, 600};
    if (setrlimit(RLIMIT_CPU, &maxLimit) < 0)
    {
        err_sys("set limit error");
    }
}