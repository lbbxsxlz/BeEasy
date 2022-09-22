# 用户态(userspace)访问MMIO内存

## 编译
```
make clean;make
```

## 示例
```
 memPhy2Virt -r -a $address
 memPhy2Virt -s -a $address -l $length
 memPhy2Virt -w -a $address -v $value
 ```
 -r stand for read
 
 -w stand for write

 -s stand for show 
