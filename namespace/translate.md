## Namespaces in operation

### part 1: namespaces overview

https://lwn.net/Articles/531114/

Linux 3.8合并窗口看到了Eric Biederman大量用户命名空间和相关补丁的接受度。 虽然仍有一些细节需要完成 - 例如，许多Linux文件系统尚未识别用户命名空间 - 用户命名空间的实现现在功能完备。

由于多种原因，完成用户命名空间工作是一个里程碑。首先，这项工作代表了迄今为止最复杂的命名空间实现之一的完成，事实证明，自实现用户命名空间的第一步（在Linux 2.6.23中）已经过去了大约五年。其次，命名空间工作目前处于“稳定点”，大多数现有命名空间的实现或多或少都是完整的。这并不意味着对名称空间的工作已经完成：将来可能会添加其他名称空间，并且可能会对现有名称空间进行进一步扩展，例如为内核日志添加名称空间隔离。最后，最近用户命名空间实现的变化在如何使用命名空间方面改变了游戏规则：从Linux 3.8开始，非特权进程可以创建用户命名空间，在这些命名空间中他们拥有完全权限，这反过来允许任何其他要在用户命名空间内创建的命名空间类型。

因此，目前的时刻似乎是一个很好的点，可以概述命名空间和实用的命名空间API。 这是一系列文章中的第一篇：在本文中，我们提供了当前可用命名空间的概述; 在后续文章中，我们将展示命名空间API如何在程序中使用。

#### The namespaces

目前，Linux实现了六种不同类型的命名空间。 每个命名空间的目的是将一个特定的全局系统资源包装在一个抽象中，使得它在命名空间内的进程看来它们拥有自己独立的全局资源实例。 命名空间的总体目标之一是支持容器的实现，容器是轻量级虚拟化（以及其他用途）的工具，它为一组进程提供了一个错觉，即它们是系统上唯一的进程。

在下面的讨论中，我们按照实现它们的顺序（或者至少是完成实现的顺序）呈现命名空间。 括号中列出的CLONE_NEW *标识符是在使用我们将在后续文章中描述的与命名空间相关的API（clone（），unshare（）和setns（））时用于标识命名空间类型的常量的名称。

挂载命名空间（CLONE_NEWNS，Linux 2.4.19）隔离了一组进程看到的文件系统挂载点集。 因此，不同安装命名空间中的进程可以具有文件系统层次结构的不同视图。 通过添加mount命名空间，mount（）和umount（）系统调用将停止在系统上所有进程可见的全局挂载点集上运行，而是执行仅影响与调用进程关联的挂载命名空间的操作。

mount命名空间的一个用途是创建类似于chroot jails的环境。 但是，与使用chroot（）系统调用相比，mount命名空间是一个更安全，更灵活的工具。 mount命名空间的其他更复杂的用法也是可能的。 例如，可以在主从关系中设置单独的安装命名空间，以便挂载事件自动从一个命名空间传播到另一个命名空间; 例如，这允许安装在一个命名空间中的光盘设备自动出现在其他命名空间中。

Mount命名空间是第一种在Linux上实现的命名空间，出现在2002年。这个事实说明了相当通用的“NEWNS”名字对象（“新命名空间”的缩写）：当时似乎没有人在想其他 ，将来可能需要不同类型的命名空间。

UTS命名空间（CLONE_NEWUTS，Linux 2.6.19）隔离了uname（）系统调用返回的两个系统标识符 -  nodename和domainname; 使用sethostname（）和setdomainname（）系统调用设置名称。 在容器的上下文中，UTS名称空间功能允许每个容器具有自己的主机名和NIS域名。 这对于基于这些名称定制其操作的初始化和配置脚本非常有用。 术语“UTS”派生自传递给uname（）系统调用的结构的名称：struct utsname。 该结构的名称又来自“UNIX分时系统”。

IPC命名空间（CLONE_NEWIPC，Linux 2.6.19）隔离了某些进程间通信（IPC）资源，即System V IPC对象和（自Linux 2.6.30）POSIX消息队列。 这些IPC机制的共同特征是IPC对象由文件系统路径名以外的机制识别。 每个IPC名称空间都有自己的一组System V IPC标识符和自己的POSIX消息队列文件系统。

PID命名空间（CLONE_NEWPID，Linux 2.6.24）隔离进程ID号空间。 换句话说，不同PID名称空间中的进程可以具有相同的PID。 PID命名空间的主要优点之一是容器可以在主机之间迁移，同时为容器内的进程保留相同的进程ID。 PID命名空间还允许每个容器具有自己的init（PID 1），即管理各种系统初始化任务的“所有进程的祖先”，并在终止时收获孤立的子进程。

从特定PID名称空间实例的角度来看，进程有两个PID：名称空间内的PID，以及主机系统上名称空间外的PID。 PID命名空间可以嵌套：一个进程将为层次结构的每个层提供一个PID，从它所驻留的PID命名空间开始到根PID命名空间。 进程可以看到（例如，通过/ proc / PID查看并使用kill（）发送信号）仅包含在其自己的PID名称空间中的进程以及嵌套在该PID名称空间下方的名称空间。

网络名称空间（CLONE_NEWNET，在Linux 2.4.19 2.6.24中启动，主要由Linux 2.6.29完成）提供与网络相关的系统资源的隔离。 因此，每个网络命名空间都有自己的网络设备，IP地址，IP路由表，/ proc / net目录，端口号等。

网络命名空间从网络角度使容器变得有用：每个容器可以拥有自己的（虚拟）网络设备和自己的应用程序，这些应用程序绑定到每个命名空间的端口号空间; 主机系统中合适的路由规则可以将网络分组引导到与特定容器相关联的网络设备。 因此，例如，可以在同一主机系统上具有多个容器化Web服务器，每个服务器在其（每个容器）网络命名空间中绑定到端口80。

用户命名空间（CLONE_NEWUSER，在Linux 2.6.23中启动，在Linux 3.8中完成）隔离用户和组ID号空间。 换句话说，进程的用户和组ID在用户命名空间的内部和外部可以是不同的。 这里最有趣的情况是进程可以在用户命名空间外具有普通的非特权用户ID，同时在命名空间内具有用户ID 0。 这意味着该进程对用户命名空间内的操作具有完全root权限，但对于命名空间外的操作没有特权。

从Linux 3.8开始，非特权进程可以创建用户命名空间，这为应用程序开辟了一系列有趣的新可能性：由于非特权进程可以在用户命名空间内保存root权限，因此非特权应用程序现在可以访问以前仅限于 根。 Eric Biederman为使用户命名空间实现安全和正确付出了很多努力。 然而，这项工作所带来的变化是微妙而广泛的。 因此，可能发生这样的情况：用户名称空间具有一些尚未知的安全问题，这些问题在未来仍有待发现和修复。

#### Concluding remarks

自实施第一个Linux命名空间以来，它已经过去了大约十年。 从那时起，命名空间概念已经扩展到一个更通用的框架，用于隔离范围以前在系统范围内的一系列全局资源。 因此，命名空间现在以容器的形式提供了完整轻量级虚拟化系统的基础。 随着名称空间概念的扩展，相关的API已经从单个系统调用（clone（））和一个或两个/ proc文件增长 - 包括许多其他系统调用和/ proc下的更多文件。 该API的详细信息将构成本文后续主题。



### part2: the namespaces API

https://lwn.net/Articles/531381/

#### demo code

##### demo_uts_namespaces.c

__demonstrate the use of UTS namespaces__

```c
/* demo_uts_namespaces.c

   Copyright 2013, Michael Kerrisk
   Licensed under GNU General Public License v2 or later

   Demonstrate the operation of UTS namespaces.
*/
#define _GNU_SOURCE
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* A simple error-handling function: print an error message based
   on the value in 'errno' and terminate the calling process */

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

static int              /* Start function for cloned child */
childFunc(void *arg)
{
    struct utsname uts;

    /* Change hostname in UTS namespace of child */

    if (sethostname(arg, strlen(arg)) == -1)
        errExit("sethostname");

    /* Retrieve and display hostname */

    if (uname(&uts) == -1)
        errExit("uname");
    printf("uts.nodename in child:  %s\n", uts.nodename);

    /* Keep the namespace open for a while, by sleeping.
       This allows some experimentation--for example, another
       process might join the namespace. */
     
    sleep(100);

    return 0;           /* Terminates child */
}

#define STACK_SIZE (1024 * 1024)    /* Stack size for cloned child */

static char child_stack[STACK_SIZE];

int
main(int argc, char *argv[])
{
    pid_t child_pid;
    struct utsname uts;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <child-hostname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create a child that has its own UTS namespace;
       the child commences execution in childFunc() */

    child_pid = clone(childFunc, 
                    child_stack + STACK_SIZE,   /* Points to start of 
                                                   downwardly growing stack */ 
                    CLONE_NEWUTS | SIGCHLD, argv[1]);
    if (child_pid == -1)
        errExit("clone");
    printf("PID of child created by clone() is %ld\n", (long) child_pid);

    /* Parent falls through to here */

    sleep(1);           /* Give child time to change its hostname */

    /* Display the hostname in parent's UTS namespace. This will be 
       different from the hostname in child's UTS namespace. */

    if (uname(&uts) == -1)
        errExit("uname");
    printf("uts.nodename in parent: %s\n", uts.nodename);

    if (waitpid(child_pid, NULL, 0) == -1)      /* Wait for child */
        errExit("waitpid");
    printf("child has terminated\n");

    exit(EXIT_SUCCESS);
}
```



##### ns_exec.c

__join a namespace using `setns()` and execute a command__

```c
#define _GNU_SOURCE
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* A simple error-handling function: print an error message based
   on the value in 'errno' and terminate the calling process */

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

int
main(int argc, char *argv[])
{
    int fd;

    if (argc < 3) {
        fprintf(stderr, "%s /proc/PID/ns/FILE cmd [arg...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);   /* Get descriptor for namespace */
    if (fd == -1)
        errExit("open");

    if (setns(fd, 0) == -1)         /* Join that namespace */
        errExit("setns");

    execvp(argv[2], &argv[2]);      /* Execute a command in namespace */
    errExit("execvp");
}
```





## UTS namespaces

介绍utsname名称空间。 而不是单个system_utsname
包含主机名域名等，进程可以请求它
要克隆的uts信息的副本。 数据将从中复制
它是原创的，但进程不会看到任何进一步的变化
这不是它的孩子，反之亦然。

例如，这对于vserver / openvz很有用，它现在可以克隆
每个新虚拟服务器的新uts名称空间。

这个补丁集是基于Kirill Korotaev的Mar 24提交的
评论（特别是来自James Morris和Eric Biederman）的评论
帐户。

附上一些性能结果。 我是否感到好奇
将task_struct-> uts_ns指针放在里面是值得的

ifdef CONFIG_UTS_NS。 结果表明，将其留在何时

CONFIG_UTS_NS = n具有可忽略的性能影响，因此就是这样
接近这个补丁需要。



## IPC namespace

此线程中的补丁添加了IPC命名空间功能
另外还包含在-mm树UTS命名空间中。

此补丁集允许取消共享IPC并具有私有集
在命名空间内的IPC对象（sem，shm，msg）。 基本上，它是
容器功能的另一个构建块。



## PID namespaces





## User namespaces

https://lwn.net/Articles/528078/

用户命名空间在Linux上工作的结果已经很长时间了，可能是因为它们是到目前为止已经添加到内核中的各种命名空间中最复杂的。 当Linux 2.6.23（2007年末发布）为clone（）和unshare（）系统调用添加了CLONE_NEWUSER标志时，实现的第一部分开始出现。 通过Linux 2.6.29，该标志在clone（）系统调用中也变得有意义。 但是，到目前为止，完全实施所需的许多部分仍然缺席。

我们上次查看了4月份的用户命名空间，当时Eric Biederman正在努力将大量补丁推入内核，目标是使实现更接近完成。 Eric现在致力于将更多补丁推入内核，目标是在Linux 3.8中或多或少完全实现用户命名空间。 因此，似乎是时候再看看这项工作了。 但是，首先，可能需要对用户命名空间进行简要回顾。

用户命名空间允许用户名和组ID的每个命名空间映射。 在容器的上下文中，这意味着用户和组可能拥有容器内某些操作的权限，而不具有容器外的这些权限。 （换句话说，进程在用户命名空间内的操作功能集可能与主机系统中的功能集完全不同。）用户命名空间的一个特定目标是允许进程具有操作的root权限 在容器内部，同时在托管容器的更广泛的系统上是一个正常的非特权进程。

为了支持这种行为，每个进程的用户ID实际上都有两个值：一个在容器内，另一个在容器外。类似的评论也适用于组ID。这种二元性是通过维护用户ID的每用户命名空间映射来实现的：每个用户命名空间都有一个表，该表将主机系统上的用户ID映射到命名空间中的相应用户ID。通过写入和读取/ proc / PID / uid_map伪文件来设置和查看此映射，其中PID是用户命名空间中某个进程的进程ID。因此，例如，主机系统上的用户ID 1000可能被映射到命名空间内的用户ID 0;因此，用户ID为1000的进程将是主机系统上的普通用户，但在命名空间内具有root权限。如果没有为主机系统上的特定用户ID提供映射，则在命名空间内，用户ID将映射到文件/ proc / sys / kernel / overflowuid中提供的值（此文件中的默认值为65534 ）。我们之前的文章详细介绍了实现。

值得注意的另一点是，前一段中给出的描述从单个用户命名空间的角度来看待事物。 但是，可以嵌套用户命名空间，并在每个嵌套级别应用用户和组ID映射。 这意味着进程可能在其所属的每个嵌套用户命名空间中具有不同的用户和组ID。

Eric已经在即将到来的3.8合并窗口中汇集了许多与命名空间相关的补丁集。 其中最主要的是完成用户命名空间基础结构的主要部分的集合。 通过此集合中的更改，非特权进程现在可以创建新的用户命名空间（使用clone（CLONE_NEWUSER））。 埃里克说，这是安全的，因为：

现在我们已经完成了对本地用户命名空间中具有uid == 0和gid == 0的内核中的每个权限检查，不再添加任何特殊权限。 即使在本地用户命名空间中拥有完整的上限也是安全的，因为功能与本地用户命名空间相关，并且不会赋予意外权限。

Eric在这里提出的要点是继续工作（在我们之前的文章中描述）在内核中实现kuid_t和kgid_t类型，以及将各种调用转换为able（）到其命名空间模拟，ns_capable（）， 用户命名空间内的用户ID为零不再授予命名空间外的特殊权限。 （able（）是检查进程是否具有功能的内核函数; ns_capable（）检查进程是否具有命名空间内的功能。）

新用户命名空间的创建者从命名空间内的一整套允许和有效功能开始，无论其主机系统上的用户ID或功能如何。 因此，创建过程具有root权限，用于在命名空间内设置环境，以准备在命名空间内创建或添加其他进程。 除此之外，这意味着用户命名空间（或实际上在命名空间内具有适当功能的任何进程）的（非特权）创建者可以反过来创建所有其他类型的命名空间，例如网络，装载和PID命名空间（这些操作） 需要CAP_SYS_ADMIN功能）。 由于创建这些命名空间的效果仅限于用户命名空间的成员，因此不会在主机系统中造成任何损坏。

Eric补丁中其他值得注意的用户空间更改包括扩展unshare（）系统调用，以便可以使用它来创建用户命名空间，以及允许进程使用setns（）系统调用进入现有用户命名空间的扩展。

查看本系列中的其他一些补丁，可以了解为了创建用户命名空间的可行实现，必须处理的一些细节是多么微妙。 例如，其中一个补丁处理set-user-ID（和set-group-ID）程序的行为。 当执行set-user-ID程序时（通过execve（）系统调用），进程的有效用户ID将更改为与可执行文件的用户ID相匹配。 当用户命名空间内的进程执行set-user-ID程序时，效果是将进程在命名空间内的有效用户ID更改为为文件用户ID映射的任何值。 返回上面使用的示例，其中主机系统上的用户ID 1000映射到命名空间内的用户ID 0，如果用户命名空间内的进程执行用户ID 1000拥有的set-user-ID程序，则该进程将 假设有效用户ID为0（在命名空间内）。

但是，如果文件用户ID在命名空间内没有映射，应该怎么做？ 一种可能性是execve（）调用失败。 但是，Eric的补丁实现了另一种方法：在这种情况下，set-user-ID位被忽略，以便执行新程序，但进程的有效用户ID保持不变。 Eric的推理是，这反映了执行set-user-ID程序的语义，该程序驻留在使用MS_NOSUID标志挂载的文件系统上。 自Linux 2.4以来，这些语义已经存在，因此应该对这种行为的内核代码路径进行充分测试。

Eric的补丁集中另一个值得注意的工作涉及/ proc / PID / ns目录中的文件。此目录包含进程所属的每种类型命名空间的一个文件（因此，对于每个进程，都有文件ipc，mnt，net，pid，user和uts）。这些文件已经有几个目的。将其中一个文件的打开文件描述符传递给setns（）允许进程加入现有的命名空间。为其中一个文件保存一个打开的文件描述符，或者将其中一个文件绑定到文件系统中的某个其他位置，即使命名空间的所有当前成员都终止，也会使命名空间保持活动状态。除此之外，后一特征允许零碎地构造容器的内容物。在Eric最近的系列中使用此补丁，现在每个命名空间创建一个/ proc inode，而/ proc / PID / ns文件则实现为引用该inode的特殊符号链接。实际的结果是，如果两个进程位于相同的用户命名空间中，则在相应的/ proc / PID / ns / user文件上调用stat（）将返回相同的inode编号（在返回的stat的st_ino字段中）结构体）。这提供了一种机制，用于发现两个进程是否在同一名称空间中，这是一个长期请求的功能。

本文仅介绍了用于完成用户命名空间实现的补丁集。 然而，与此同时，Eric正在向主线推送一些相关的补丁集，包括：更改网络堆栈，以便用户命名空间root用户可以创建网络命名空间：增强和清理PID命名空间代码， 除其他外，添加对NL名称空间的unshare（）和setns（）支持; mount命名空间代码的增强功能，允许用户命名空间root用户调用chroot（）并创建和操作mount命名空间; 以及一系列补丁，它们将用户命名空间的支持添加到许多尚未提供该支持的文件系统中。

值得强调的是Eric在用户命名空间工作的文档补丁中提到的一点，并在私人邮件中进行了详细阐述。 除了支持容器的实用性之外，用户命名空间工作背后还有另一个重要的驱动力：释放由set-user-ID和set-group-ID程序强加的“手铐”的UNIX / Linux API。 内核提供的许多用户空间API都只是root用户，只是为了防止意外或恶意扭曲特权程序的运行时环境的可能性，导致这些程序被混淆为做一些他们没有设计的东西 去做。 通过限制root权限对用户命名空间的影响，并允许非特权用户创建用户命名空间，现在可以让非root用户访问以前仅限于root用户的有趣功能。

有一些Acked-by：邮件响应Eric的补丁而发送，还有一些小问题，但这些补丁在很大程度上没有发表评论，也没有人提出反对意见。 这似乎可能是因为补丁在相当长的一段时间内以某种形式存在，并且Eric已经付出了相当大的努力来解决在用户命名空间工作期间之前提出的异议。 因此，似乎有一个很好的机会，即Eric在当前打开的3.8合并窗口中合并补丁的请求将成功，并且用户命名空间的完整实现现在非常接近现实。