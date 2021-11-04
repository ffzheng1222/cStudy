

KERNEL_DIR = /home/farsight/S5PV210/kernel/linux-3.0.8

USER_DIR = $(shell pwd)

driver1 = hello_fun_drv
driver2 = fun

all:
	make  -C  $(KERNEL_DIR)  M=$(USER_DIR) modules

install:
	cp  *.ko  /opt/rootfs/drv_module

clean:
	make  -C  $(KERNEL_DIR)  M=$(USER_DIR) clean 

obj-m  =  $(driver1).o  $(driver2).o 
