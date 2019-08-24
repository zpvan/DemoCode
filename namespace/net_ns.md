## NET_NS

基于ip工具进行介绍

创建新的net_ns

```sh
$ sudo ip netns add netns1
```



会在/var/run/netns/多出一个文件

```sh
$ ls -l /var/run/netns/
total 0
-r--r--r-- 1 root root 0 Aug 24 02:25 netns1
```



查看新net_ns的网络配置

```sh
$ sudo ip netns exec netns1 ip link list
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    
$ sudo ip netns exec netns1 ifconfig
$
```



试试ping自己

```sh
$ sudo ip netns exec netns1 ping 127.0.0.1
connect: Network is unreachable
```



使能lo

```sh
$ sudo ip netns exec netns1 ip link set dev lo up
$ sudo ip netns exec netns1 ping 127.0.0.1
PING 127.0.0.1 (127.0.0.1) 56(84) bytes of data.
64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.031 ms
```



查看配置

```sh
$ sudo ip netns exec netns1 ip link list
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
$ sudo ip netns exec netns1 ifconfig
lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 4  bytes 336 (336.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4  bytes 336 (336.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```



给新的net_ns配置虚拟网卡

```sh
$ sudo ip link add veth0 type veth peer name veth1
$ sudo ip link set veth1 netns netns1
```



查看配置

```sh
$ sudo ip netns exec netns1 ip link list
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
5: veth1@if6: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 7e:e6:a1:01:ed:04 brd ff:ff:ff:ff:ff:ff link-netnsid 0
$ sudo ip netns exec netns1 ifconfig
lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 4  bytes 336 (336.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 4  bytes 336 (336.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

```



使能虚拟网卡

```sh
$ sudo ip netns exec netns1 ifconfig veth1 10.1.1.1/24 up
$ sudo ifconfig veth0 10.1.1.2/24 up
```



新的net_ns可以ping通host了

```sh
$ sudo ip netns exec netns1 ping 10.1.1.2
PING 10.1.1.2 (10.1.1.2) 56(84) bytes of data.
64 bytes from 10.1.1.2: icmp_seq=1 ttl=64 time=0.085 ms
```



但是不能ping到外网

```sh
$ sudo ip netns exec netns1 ping www.baidu.com
ping: www.baidu.com: Name or service not known
```



配置路由表

