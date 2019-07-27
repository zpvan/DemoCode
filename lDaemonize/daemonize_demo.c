#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

void mdaemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    // Clear file creation mask.
    umask(0);

    // Get maximun number of file descriptors.
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        printf("%s: can't get file limit\n", cmd);
        exit(0);
    }

    // Become a session leader to lose controlling TTY.
    if ((pid = fork()) < 0)
    {
        printf("%s: can't fork\n", cmd);
        exit(0);
    } else if (pid != 0)
    {
        // parent
        exit(0);
    }

    // child
    printf("child pid = %ld\n", (long)getpid());
    setsid();

    // Ensure future opens won't allocate controlling TTYs.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        printf("%s: can't ignore SIGHUP\n", cmd);
        exit(0);
    }

    if ((pid = fork()) < 0)
    {
        printf("%s: can't fork 2\n", cmd);
        exit(0);
    } else if (pid != 0)
    {
        exit(0); // child
    }

    // second child
    printf("second child pid = %ld\n", (long)getpid());
    /*
     * Change the current working directory to the root so
     * we won't prevent file system from being unmounted.
     */
    if (chdir("/") < 0)
    {
        printf("%s: can't change directory to /\n", cmd);
        exit(0);
    }

    // Close all open file descriptors.
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    // Attach file descriptors 0, 1, and 2 to /dev/null.
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    // Initialize the log file.
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    printf("main pid = %ld\n", (long)getpid());
    mdaemonize("mdaemonize");
    return 0;
}