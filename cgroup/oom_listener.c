/**
 * 1. 利用函数eventfd()创建一个efd
 * 2. 打开文件memory.oom_control，得到ofd
 * 3. 往cgroup.event_control中写入这么一串：<efd> <ofd>
 * 4. 通过读efd得到通知，然后打印一句话到终端
 **/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static inline void die(const char *msg) {
    fprintf(stderr, "error: %s : %s(%d)\n", msg, strerror(errno), errno);
    exit(-1);
}

#define BUFFSIZE 256

int main(int argc, char *argv[]) {
    char buf[BUFFSIZE];
    int efd, cfd, ofd;
    uint64_t u;

    if ((efd = eventfd(0, 0)) == -1) {
        die("eventfd");
    }

    snprintf(buf, BUFFSIZE, "%s/%s", argv[1], "cgroup.event_control");
    if ((cfd = open(buf, O_WRONLY)) == -1) {
        die("cgroup.event_control");
    }

    snprintf(buf, BUFFSIZE, "%s/%s", argv[1], "memory.oom_control");
    if ((ofd = open(buf, O_RDONLY)) == -1) {
        die("memory.oom_control");
    }

    snprintf(buf, BUFFSIZE, "%d %d", efd, ofd);
    if (write(cfd, buf, strlen(buf)) == -1) {
        die("write cgroup.event_control");
    }

    if (close(cfd) == -1) {
        die("close cgroup.event_control");
    }

    for (;;) {
        if (read(efd, &u, sizeof(uint64_t)) != sizeof(uint64_t)) {
            die("read eventfd");
        }
        printf("mem_group oom event received(%lu)!\n", u);
    }

    return 0;
}