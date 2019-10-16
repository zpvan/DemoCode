### ffmpeg

配置和编译

```shell
cd ffmpeg-$(version) # 目前version=3.4
./config_hi3531d.sh # 裁剪ffmpeg的编译模块,保证功能够用的情况下,尽量减少lib_size
make
make install
```



### mediatransaction

```
基础结构介绍
Server: 服务. 譬如有ipav, usb-camera, doggle, airplay, miracast等都属于单个的服务
MediaTransactionUdp: 服务的对外端口. 该服务通过其预订的端口与其他模块沟通
Router: 路由. 我的意思是想, 一条message经过router够会送给具体的工作者
Stream: 流. 也是具体的工作者, 譬如每路ipe的码流, 都对应唯一stream, 每个camera, 也是对应唯一的stream
```



### myucam

```
除了mediatransaction基本的几个模块外
UcamScanner: 扫描器. 监测usb口上camera的插拔行为, 给router反馈消息
FFCamera: 基于ffmpeg的camera. 可以从它里边读出摄像头采集到的码流, 被stream包着
```



### myipav

```
除了mediatransaction基本的几个模块外
IpavInput: 码流接收器. 接收ipe的组播码流, 被stream包着
```

