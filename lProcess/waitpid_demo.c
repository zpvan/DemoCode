#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    printf("parent pid = %ld\n", (long)getpid());

    if ((pid = fork()) < 0)
        printf("fork error\n");
    else if (pid == 0) { // first child
        printf("first child pid = %ld\n", (long)getpid());
        printf("first child, parent pid = %ld\n", (long)getppid());
        if ((pid = fork()) < 0)
            printf("fork error\n");
        else if (pid > 0)
            exit(0); // first child exit

        printf("second child pid = %ld\n", (long)getpid());
        // second child ==> the child of the first child
        sleep(2);
        printf("second child, parent pid = %ld\n", (long)getppid());
        exit(1);
    }
    
    printf("parent wait pid = %d\n", pid);
    if (waitpid(pid, NULL, 0) != pid) // wait first child
        printf("wait_pid error\n");

    printf("parent wait pid = %d done\n", pid);

    exit(0);
}