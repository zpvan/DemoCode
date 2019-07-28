#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 4096

int main(int argc, char *argv[])
{
    int n;
    int fd[2];
    pid_t pid;
    char line[MAXLINE];

    if (pipe(fd) < 0)
    {
        printf("pipe error\n");
    }

    if ((pid = fork()) < 0)
        printf("fork error\n");
    else if (pid > 0)
    {
        // parent
        // close read pipe
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    } else 
    {
        // child
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
    }

    exit(0);
}
