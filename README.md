# 实现一个工具集使定位问题或解决问题更简单快捷。

## EthernetDemo
[构建以太网帧并发送](ethernetDemo/sendEther.c)

[接受以太网帧并打印内容](ethernetDemo/recvEther.c)

## 以太网帧文件传输
[文件传送](fileTransmit/README)
文件传输功能已经实现，为了保证数据的完整性，每发送一帧以太网帧均需要回复，导致文件传输的速率无法提升。
TODO：提升文件传输的效率

## 用户态MMIO物理地址映射到虚拟地址
[mmio map](memPhy2Virt/README.md)

## Window系统socket创建简单的基于TCP的C/S demo
[server](sockets_tcp/server.c)
[client](sockets_tcp/client.c)

## windows系统上getopt的实现
[getopt](getopt4windows/README.md)

## cJson demo, 查找关键字中的某个键值
[json_read](jsonRead/jsonRead.c)
