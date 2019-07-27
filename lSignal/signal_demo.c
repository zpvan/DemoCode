#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void sig_usr(int);

int main(int argc, char *argv)
{
    printf("main pid = %ld\n", (long)getpid());
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        printf("can't catch SIGUSR1\n");

    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        printf("can't catch SIGUSR2\n");

    if (signal(SIGTERM, sig_usr) == SIG_ERR)
        printf("can't catch SIGTERM\n");

    for (;;) {
        printf("pause begin\n");
        pause();
        printf("pause end\n");
    }
        

    return 0;
}

static void sig_usr(int signo)
{
    printf("sig_usr pid = %ld\n", (long)getpid());
    if (signo == SIGUSR1)
        printf("received SIGUSER1\n");

    if (signo == SIGUSR2)
        printf("received SIGUSER2\n");

    if (signo == SIGTERM)
    {
        printf("received SIGTERM\n");
        exit(0);
    }
}