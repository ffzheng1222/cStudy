

#编译模块必须先将linux内核源码编译一次


## 1. 指定内核源码的路径

CC = arm-none-linux-gnueabi-gcc
KERNEL_DIR = /home/farsight/S5PV210/kernel/linux-3.0.8

USER_DIR = $(shell pwd)


myapp  = led_test

driver1 = platdata_led_drv
driver2 = platdata_led_dev

all:
	    make -C $(KERNEL_DIR) M=$(USER_DIR) modules
	    $(CC) -o $(myapp) $(myapp).c

install:
	    cp  *.ko  $(myapp) /opt/rootfs/drv_module


clean:
	    make -C $(KERNEL_DIR) M=$(USER_DIR) clean
	    $(RM) -rf $(myapp) .*.sw? -rf

## 2. 指定编译哪个源文件
obj-m += $(driver1).o
obj-m += $(driver2).o









