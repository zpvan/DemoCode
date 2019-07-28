#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// 定义一个给clone用的栈, 栈大小为1M
#define STACK_SIZE (1024*1024)
static char container_stack[STACK_SIZE];

char* const container_args[] = {
    "/bin/bash",
    NULL
};

int container_main()
{
    printf("Container - inside the container!\n");
    printf("Container - pid = %ld\n", (long)getpid());
    sethostname("container", 10); // 设置hostname
    // 直接执行一个shell, 以便我们观察这个进程空间里的资源是否被隔离
    execv(container_args[0], container_args);
    printf("Shouldn't be here!\n");
    return 1;
}

int main(int argc, char *argv[])
{
    printf("Parent - start a container!\n");
    // 调用clone函数, 其中传入一个函数, 还有一个栈空间的地址(为什么传尾指针, 因为栈是反着的)
    int container_pid = clone(container_main, container_stack+STACK_SIZE, 
        CLONE_NEWUTS | CLONE_NEWIPC | CLONE_NEWPID | SIGCHLD, NULL);
    // 等待子进程结束
    waitpid(container_pid, NULL, 0);
    printf("Parent - stop a container!\n");
    return 0;
}
