#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

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

int msystem(const char *cmdstring)
{
    pid_t pid;
    int status;

    if (cmdstring == NULL)
        return 1;

    if ((pid = fork()) < 0)
    {
        printf("fork error\n");
        status = -1;
    } else if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
    } else
    {
        while (waitpid(pid, &status, 0) < 0)
        {
            if (errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }
    return status;
}

int main(int argc, char *argv[])
{
    int status;

    //-------------------------------------------
    if ((status = msystem("date")) < 0)
        printf("msystem() error");
    pr_exit(status);

    //-------------------------------------------
    if ((status = msystem("nosuchcommand")) < 0)
        printf("msystem() error");
    pr_exit(status);

    //-------------------------------------------
    if ((status = msystem("who; exit 44")) < 0)
        printf("msystem() error");
    pr_exit(status);
}