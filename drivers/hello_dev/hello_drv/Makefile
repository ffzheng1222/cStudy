

## 1. 指定内核源码的路径
KERNEL_DIR = /home/farsight/S5PV210/kernel/linux-3.0.8

USER_DIR = $(shell pwd)


driver = hello_drv


all:
	make -C $(KERNEL_DIR) M=$(USER_DIR) modules
	
install:
	cp  *.ko /opt/rootfs/drv_module

clean:
	make -C $(KERNEL_DIR) M=$(USER_DIR) clean

## 2. 指定编译哪个源文件

obj-m = $(driver).o



