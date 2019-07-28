官方文档

https://lwn.net/Articles/531114/



主要是三个系统调用配合使用

* clone() - 实现进程的系统调用, 用来创建一个新的进程, 并可以通过设置参数(CLONE_NEWNS[mount namespace], CLONE_NEWUTS[uts namespace], CLONE_NEWIPC[ipc namespace], CLONE_NEWPID[pid namespace], CLONE_NET[net namespace], CLONE_NEWUSER[user namespace])达到隔离
* unshare() - 使某进程脱离某个namespace
* setns() - 把某进程加入到某个namespacez

