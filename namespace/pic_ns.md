## IPC方式

### pipe

```c
#include <unistd.h>
int pipe(int fd[2]);
```

只能在两个相关的进程之间使用, 而且这两个相关的进程还要有一个共同的创建了它们的祖先进程



### fifo

```
#include <sys/stat.h>
int mkfifo(const char *path, mode_t mode);
int mkfifoat(int fd, const char *path, mode_t mode);
```

存储在硬盘上的文件, 且mkfifo后, 允许多个进程open它



### 消息队列

```c
#include <sys/ipc.h>
key_t ftok(const char *path, int id);

#include <sys/msg.h>
int msgget(key_t key, int flag);
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
int msgsnd(int msqid, const void *ptr, size_t nbytes, int flag);
int msgrcv(int msqid, void *ptr, size_t nbytes, long type, int flag);
```



### 信号量

```c
#include <sys/sem.h>
int semget(key_t key, int nsems, int flag);
int semctl(int semid, int semnum, int cmd, ... /* union semun arg */);
int semop(int semid, struct sembuf semoparray[], size_t nops);
```



### 共享内存

```c
#include <sys/shm.h>
int shmget(key_t key, size_t size, int flag);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
void *shmat(int shmid, const void *addr, int flag);
int shmdt(const void *addr);
```



### POSIX信号量

```c
#include <semaphore.h>
sem_t *sem_open(const char *name, int oflag, ... /* mode_t mode, unsigned int value */);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);
int sem_trywait(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_init(semt_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_getvalue(sem_t *restrict sem, int *restrict valp);
#include <time.h>
int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict tsptr);
```



### Socket

```c
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```



## IPC Namespace

有三种称作XSI IPC的IPC: 消息队列, 信号量, 共享内存

可以确认的ipc_ns会隔离他们仨, 至于其他的ipc方式, 需要继续考证



ubuntu默认提供了ipc的相关工具

```sh
parallels@parallels-Parallels-Virtual-Platform:~$ ipc
ipcmk  ipcrm  ipcs
```



```sh
parallels@parallels-Parallels-Virtual-Platform:~$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status

------ Semaphore Arrays --------
key        semid      owner      perms      nsems

```

可以看出这套ipc工具, 管理了消息队列, 共享内存, 信号量



终端A利用ipcmk分别新建消息队列, 共享内存, 信号量

```sh
parallels@parallels-Parallels-Virtual-Platform:~$ ipcmk -Q
Message queue id: 32768
parallels@parallels-Parallels-Virtual-Platform:~$ ipcmk -S 9
Semaphore id: 0
parallels@parallels-Parallels-Virtual-Platform:~$ ipcmk -M 1024
Shared memory id: 65536
parallels@parallels-Parallels-Virtual-Platform:~$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x8be49631 32768      parallels  644        0            0

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x7d9558fb 65536      parallels  644        1024       0

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0x737b6e57 0          parallels  644        9
```

终端B查看

```sh
parallels@parallels-Parallels-Virtual-Platform:~$ ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x8be49631 32768      parallels  644        0            0

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status
0x7d9558fb 65536      parallels  644        1024       0

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0x737b6e57 0          parallels  644        9

parallels@parallels-Parallels-Virtual-Platform:~$ sudo unshare -i
root@parallels-Parallels-Virtual-Platform:~# ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
```

可以看到, ipc_ns是可以隔离消息队列, 共享内存, 信号量, 3种ipc的



pipe这种是无法隔离的