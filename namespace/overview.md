官方文档

https://lwn.net/Articles/531114/



主要是三个系统调用配合使用

* clone() - 实现进程的系统调用, 用来创建一个新的进程, 并可以通过设置参数(CLONE_NEWNS[mount namespace], CLONE_NEWUTS[uts namespace], CLONE_NEWIPC[ipc namespace], CLONE_NEWPID[pid namespace], CLONE_NET[net namespace], CLONE_NEWUSER[user namespace])达到隔离
* unshare() - 使某进程脱离某个namespace
* setns() - 把某进程加入到某个namespacez



clone调用的基础框架

clone_demo.c

```c
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
    // 直接执行一个shell, 以便我们观察这个进程空间里的资源是否被隔离
    execv(container_args[0], container_args);
    printf("Shouldn't be here!\n");
    return 1;
}

int main(int argc, char *argv[])
{
    printf("Parent - start a container!\n");
    // 调用clone函数, 其中传入一个函数, 还有一个栈空间的地址(为什么传尾指针, 因为栈是反着的)
    int container_pid = clone(container_main, container_stack+STACK_SIZE, SIGCHLD, NULL);
    // 等待子进程结束
    waitpid(container_pid, NULL, 0);
    printf("Parent - stop a container!\n");
    return 0;
}
```



<img src="./res/clone_demo1.png">



UTS Namespace

uts_demo.c

```c
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
        CLONE_NEWUTS | SIGCHLD, NULL);
    // 等待子进程结束
    waitpid(container_pid, NULL, 0);
    printf("Parent - stop a container!\n");
    return 0;
}
```



<img src="./res/uts_demo1.png">

* CLONE_NEWUTS需要root权限才能运行
* 子进程的hostname变成了container



IPC Namespace

ipc_demo.c

```c
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
        CLONE_NEWUTS | CLONE_NEWIPC | SIGCHLD, NULL);
    // 等待子进程结束
    waitpid(container_pid, NULL, 0);
    printf("Parent - stop a container!\n");
    return 0;
}
```



<img src="./res/ipc_demo1.png">



<img src="./res/ipc_demo2.png">

