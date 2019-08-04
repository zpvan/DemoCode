#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
//#include <sys/capability.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define TEST

// 定义一个给clone用的栈, 栈大小为1M
#define STACK_SIZE (1024*1024)
static char container_stack[STACK_SIZE];

char* const container_args[] = {
    "/bin/bash",
    NULL
};

int pipefd[2];

void set_map(char* file, int inside_id, int outside_id, int len)
{
    FILE* fd = fopen(file, "w");
    if (!fd) {
	    printf("open file(%s) failed!\n", file);
        return;
    }
    fprintf(fd, "%d %d %d", inside_id, outside_id, len);
    fclose(fd);
}

void set_uid_map(pid_t pid, int inside_id, int outside_id, int len)
{
    char file[256];
    sprintf(file, "/proc/%d/uid_map", pid);
    set_map(file, inside_id, outside_id, len);
}

void set_gid_map(pid_t pid, int inside_id, int outside_id, int len)
{
    char file[256];
    sprintf(file, "/proc/%d/gid_map", pid);
    set_map(file, inside_id, outside_id, len);
}

void get_map(char* file)
{
    FILE* fd = fopen(file, "r");
    if (!fd) {
	    printf("open file(%s) failed!\n", file);
        return;
    }
    char content[256] = {0};
    fread(content, 1, 255, fd);
    printf("%s => %s", file, content);
    fclose(fd);
}

void get_uid_map(container_pid) {
    char file[256];
    sprintf(file, "/proc/%d/uid_map", pid);
    get_map(file);    
}

void get_gid_map(container_pid) {
    char file[256];
    sprintf(file, "/proc/%d/gid_map", pid);
    get_map(file);    
}

int container_main()
{
    printf("Container - inside the container!\n");
    printf("Container: eUID = %ld, eGID = %ld, UID = %ld, GID = %ld\n",
        (long)geteuid(), (long)getegid(), (long)getuid(), (long)getgid);

    // 等待父进程通知后再往下执行 (进程间的同步)
    char ch;
    close(pipefd[1]);
    read(pipefd[0], &ch, 1);

    // set hostname
    // printf("Container [%ld] - setup hostname!\n", (long)getpid());
    // sethostname("container", 10); // 设置hostname

    // remount "/proc" to make sure the "top" and "ps" show container's information
    // system("mount -t proc proc /proc");
    // mount("proc", "/proc", "proc", 0, NULL);

    // 直接执行一个shell, 以便我们观察这个进程空间里的资源是否被隔离
    execv(container_args[0], container_args);
    printf("Shouldn't be here!\n");
    return 1;
}

int main(int argc, char *argv[])
{
    const int gid = getgid(), uid = getuid();
    printf("Parent: eUID = %ld, eGID = %ld, UID = %ld, GID = %ld\n",
        (long)geteuid(), (long)getegid(), (long)getuid(), (long)getgid);
    pipe(pipefd);

    printf("Parent - start a container!\n");
    // 调用clone函数, 其中传入一个函数, 还有一个栈空间的地址(为什么传尾指针, 因为栈是反着的)
    int container_pid = clone(container_main, container_stack+STACK_SIZE, 
     //   CLONE_NEWUTS |
     //   CLONE_NEWIPC |
     //   CLONE_NEWPID |
     //   CLONE_NEWNS |
        CLONE_NEWUSER |
        SIGCHLD, NULL);

    get_uid_map(container_pid);
    get_gid_map(container_pid);

    printf("Parent pid = %ld, Child pid = %d\n", (long)getpid(), container_pid);

    // To map the uid/gid, we need edit the /proc/PID/uid_map (or gid_map) in parent
    // The file format is, ID-inside-ns ID-outside-ns length
    // If no mapping, 
    //     the uid will be taken from /proc/sys/kernel/overflowuid
    //     the gid will be taken from /proc/sys/kernel/overflowgid
    set_uid_map(container_pid, 0, uid, 1);
    set_gid_map(container_pid, 0, gid, 1);

    printf("Parent [%ld] - user/group mapping done!\n", (long)getpid());

    get_uid_map(container_pid);
    get_gid_map(container_pid);

    // 通知子进程
    close(pipefd[1]);

    // 等待子进程结束
    waitpid(container_pid, NULL, 0);
    printf("Parent - stop a container!\n");
    return 0;
}
