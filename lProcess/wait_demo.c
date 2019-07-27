//#include "apue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void pr_exit(int status)
{
    if (WIFEXITED(status))
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("abnormal termination, signal number = %d%s\n", WTERMSIG(status),
#ifdef WCOREDUMP
        WCOREDUMP(status) ? "(core file generated)" : "");
#else
        "");
#endif
    else if (WIFSTOPPED(status))
        printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}


void main(int argc, char *argv[])
{
    pid_t pid;
    int status;

    //-------------------------------------------
    if ((pid = fork()) < 0) 
        printf("fork error\n");
    else if (pid == 0)
        exit(7); // child

    if (wait(&status) != pid) // parent
        printf("wait error\n");
    pr_exit(status);

    //-------------------------------------------
    if ((pid = fork()) < 0) 
        printf("fork error\n");
    else if (pid == 0)
        abort(); // child

    if (wait(&status) != pid) // parent
        printf("wait error\n");
    pr_exit(status);

    //-------------------------------------------
    if ((pid = fork()) < 0) 
        printf("fork error\n");
    else if (pid == 0)
        status /= 0; // child

    if (wait(&status) != pid) // parent
        printf("wait error\n");
    pr_exit(status);

    exit(0);
}