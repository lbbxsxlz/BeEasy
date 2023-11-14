# 收集并整理日常使用的工具使定位问题或解决问题更简单快捷。

## EthernetDemo
[构建以太网帧并发送](ethernetDemo/sendEther.c)

[接受以太网帧并打印内容](ethernetDemo/recvEther.c)

## Ethernet phy register read and write
[以太网Phy寄存器操作](mii/mii.c)
```
mii $ifname
mii $ifname $reg
mii $ifname $reg $val
```

## Ethernet文件传输
[文件传送](fileTransmit/README)
文件传输功能已经实现，为了保证数据的完整性，每发送一帧以太网帧均需要回复，导致文件传输的速率无法提升。
TODO：提升文件传输的效率

## 二维数组内存分配
[2d_array](2d_array/2d_array.c)

## C语言位域操作
[bitField](bitField/bitField.c)

## CRC校验算法
[crc](crc/crc32.c)

## Function Dispatch
[dispatch](dispatch/dispatch_test.c)

## linux 可执行文件格式elf format demo
[elfRead](elfRead/elf32bit.c)
[elfRead](elfRead/elf64.c)

## 程序调用shell脚本，返回shell输出结果或只关注执行结果
[execshell](execShell/execShell.c)

## 常用的文件操作
[file operation](file_operation/file.c)

## windows系统上getopt的实现
[getopt](getopt4windows/README.md)

## cJson demo, 查找关键字中的某个键值
[json_read](jsonRead/jsonRead.c)
```
jsonRead $fileName String
```
more detail please refer the [doc](jsonRead/README.md)

## 用户态MMIO物理地址映射到虚拟地址
The below is the usage, more detail please refer the [doc](memPhy2Virt/README.md)
```
 memPhy2Virt -r -a $address
 memPhy2Virt -s -a $address -l $length
 memPhy2Virt -w -a $address -v $value
 ```

## 使用realloc再次分配内存
[realloc](realloc/realloc_test.c)

## Window系统socket创建简单的基于TCP的C/S demo
[server](sockets_tcp/server.c)
[client](sockets_tcp/client.c)

## SpinLock Test 自旋锁demo用于验证CPU hang
[Spin_Lock](spinLockTest/spinLockTest.c)

## 如何使用Makefile编译静态库、动态库以及内核模块
[static and dynamic library](Makefile/library.mk)
[kernel module](Makefile/kernel_module.mk)

## 将16进制的字符串转成二进制数据
[str2hex](str2hex/str2hex.c)

## 遍历目录，按条件删除文件或目录
[walkthrough](walkthrough/walkthroughPath.c)
