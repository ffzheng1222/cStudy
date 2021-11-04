一、环境搭建
	1，获取Linux内核源码：
		1.> 官网获取  www.kernel.org
		2.> 给定：  E:\lewis\初级驱动\Linux初级驱动\1day\source\linux-3.0.8.tar.bz2
	2,编译内核源码
		1.> 将内核源码拷贝到 \\192.168.7.7\peter\s5pv210\kernel，并解压
			tar -xvf linux-3.0.8.tar.bz2
		2.> 修改Makefile，设置交叉工具链：
			195 ARCH            ?= arm
			196 CROSS_COMPILE   ?= arm-none-linux-gnueabi-
		3.> 选择对应的soc
			方法1：
				cp arch/arm/configs/s5pv210_defconfig .config
			方法2：
				make s5pv210_defconfig
		4.> make menuconfig
				注意：如果执行出错，则需要安装库：ncurses
					sudo apt-get install libncurses5-dev
			 System Type  --->   
				(0) S3C UART to use for low-level messages
			 [*] Networking support  ---> 
				Networking options  ---> 
					 [*] TCP/IP networking 
		5.> 编译内核
			make zImage -j2 或 make uImage -j2
			
		    编译好的镜像在arch/arm/boot/zImage
	3，运行开发板
	
		1.> 下载启动内核
			
			pc端:ubuntu中：
				 cp -arf arch/arm/boot/zImage /tftpboot/
			开发板端：配置uboot启动参数
				FS210 # set  serverip 192.168.7.7     				//指定tftp服务器的IP
				FS210 # set ipaddr 192.168.7.5		  				//指定开发板IP
				FS210 # set bootcmd tftp 40800000 zImage\;bootm 40800000
				FS210 # save
		2.> 挂载根文件系统 nfs共享
			pc端:ubuntu中：
				 E:\lewis\初级驱动\Linux初级驱动\1day\source\rootfs_fs210.tgz  拷贝到 ubuntu中的/opt/
				 a,解压：
					sudo tar -xvf rootfs_fs210.tgz
				 b,配置：
					sudo vi /etc/exports
					添加下面内容：
						 /opt/rootfs       *(subtree_check,rw,no_root_squash,async)
				 c,重启服务：
						sudo /etc/init.d/nfs-kernel-server restart
						sudo exportfs -a
			开发板端：配置uboot启动参数
				FS210 # set bootargs root=nfs nfsroot=192.168.7.7:/opt/rootfs ip=192.168.7.5 init=/linuxrc console=ttySAC0,115200
				FS210 # save
					解释：
						bootargs 							//uboot的中的一个变量，会传递给内核
						root=nfs							//告诉内核通过哪种方式挂载根文件系统
						nfsroot=192.168.7.7:/opt/rootfs		//具体的挂载路径
						ip=192.168.7.5						//具体开发板的IP
						console=ttySAC0,115200				//从哪个设备打印信息
		
		3.> 重写启动开发板	
			VFP support v0.3: implementor 41 architecture 3 part 30 variant c rev 2
			VFS: Cannot open root device "nfs" or unknown-block(0,0)
			Please append a correct "root=" boot option; here are the available partitions:
			Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)
			[<80031564>] (unwind_backtrace+0x0/0xf0) from [<8020656c>] (panic+0x70/0x19c)
			[<8020656c>] (panic+0x70/0x19c) from [<80008dac>] (mount_block_root+0x1d4/0x298)
			[<80008dac>] (mount_block_root+0x1d4/0x298) from [<80009124>] (prepare_namespace+0x164/0x1bc)
			[<80009124>] (prepare_namespace+0x164/0x1bc) from [<80008aa0>] (kernel_init+0xe4/0x118)
			不好意思出错了，原因是在编译内核时，没有支持文件系统：nfs
			重新执行 make menuconfig
				General setup  --->     
					[*] Configure standard kernel features (for small systems)
				Kernel Features  --->        
					Memory split (3G/1G user/kernel split)  --->
				[ * ] Networking support  --->     
				Networking options  ---> 
					   <*>Packet sockets 
					   <*> Unix domain sockets 
						[*] TCP/IP networking
							[*]   IP: multicasting  
							[*]		IP: kernel level autoconfiguration
							[*]     IP: BOOTP support
				Device Drivers  --->
						[*] Network device support  --->
							[*]   Ethernet driver support (NEW)  --->
							   [*]   Ethernet (10 or 100Mbit)  --->
										<*>   DM9000 support
				File systems  --->
					[*] Network File Systems (NEW)  --->
						<*>   NFS client support
						[*]     NFS client support for NFS version 3
						[*]       NFS client support for the NFSv3 ACL protocol extension
						[*]   Root file system on NFS
						
		4.> Fs210之网卡驱动添加
			    平台代码修改:(下面的代码在拷贝的时候千万注意中文状态哦)
					vi arch/arm/mach-s5pv210/mach-smdkv210.c
			

				4.1修改结构体数组smdkv210_dm9000_resources为：
				static struct resource smdkv210_dm9000_resources[] = {
					[0] = {
						.start	= 0x88000000,            //地址端口
						.end	= 0x88000000 + 0x3,
						.flags	= IORESOURCE_MEM,
					},
					[1] = {
						.start	= 0x88000000 + 0x4,       //数据端口
						.end	= 0x88000000 + 0x4 + 0x3,
						.flags	= IORESOURCE_MEM,
					},
					[2] = {
					.start	= IRQ_EINT(10),           //中断号
						.end	= IRQ_EINT(10),
						.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
					},
				};

				4.2修改函数smdkv210_dm9000_init为：
				static void __init smdkv210_dm9000_init(void)
				{
						 /*时序配置:需要技术支持*/
					unsigned int tmp;
					gpio_request(S5PV210_MP01(1), "nCS1");
					s3c_gpio_cfgpin(S5PV210_MP01(1), S3C_GPIO_SFN(2));
					gpio_free(S5PV210_MP01(1));
						tmp = (5 << S5P_SROM_BCX__TACC__SHIFT);
					__raw_writel(tmp, S5P_SROM_BC1);
					tmp = __raw_readl(S5P_SROM_BW);
					tmp &= (S5P_SROM_BW__CS_MASK << S5P_SROM_BW__NCS1__SHIFT);
					tmp |= (1 << S5P_SROM_BW__NCS1__SHIFT);
					__raw_writel(tmp, S5P_SROM_BW);
				}	
			
			
				4.3修改网卡的mac地址:
					static struct dm9000_plat_data smdkv210_dm9000_platdata = {
							.flags = DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM,
							.dev_addr    = { 0x00, 0x09, 0xc0, 0xff, 0xec, 0xee }, // 设置为座位号码
					};
			
		  重新编译内核:
				make zImage -j2
		  将内核镜像zImage移植到开发板,并启动开发板
			VFS: Mounted root (nfs filesystem) on device 0:10.
			Freeing init memory: 116K
			==================^_^ Finished===================

			Please press Enter to activate this console.
			看到上面的结果，恭喜你，你的内核移植成功了！！
二、Linux模块编程
	1、模块编写要素
		//头文件
		#include <linux/init.h>
		#include <linux/module.h>

		//模块加载函数
		static int __init hello_init(void)
		{
			printk("============%s============\n",__func__);
			return 0;
		}
		//模块卸载函数
		static void __exit hello_exit(void)
		{
			printk("============%s============\n",__func__);
		}


		//模块加载函数和模块卸载函数的声明
		module_init(hello_init);
		module_exit(hello_exit);

		//添加GPL认证
		MODULE_LICENSE("GPL");
	2、模块编译:
		Makefile:
			#编译模块必须先将linux内核源码编译一次

			#指定内核源码的路径
			KERNEL_DIR = /home/peter/s5pv210/kernel/linux-3.0.8
			CUR_DIR = $(shell pwd)

			driver = hello_drv
			
			all:
					make -C $(KERNEL_DIR) M=$(CUR_DIR) modules
			install:
					cp  hello_drv.ko /opt/rootfs/drv_module
			clean:
					make -C $(KERNEL_DIR) M=$(CUR_DIR) clean

			#指定编译哪个源文件
			obj-m = $(driver).o
	3、模块装载和卸载
			
			[root@farsight /drv_module]# insmod hello_drv.ko
			============hello_init============
			[root@farsight /drv_module]# rmmod hello_drv
			============hello_exit============
			[root@farsight /drv_module]# insmod hello_drv.ko
			============hello_init============
			[root@farsight /drv_module]# lsmod
			hello_drv 747 0 - Live 0x7f004000
			[root@farsight /drv_module]#

	4、模块传参和模块调用
		1> 模块传参
			 源代码：
				int a = 123;
				char * str = "hello";

				//模块加载函数
				static int __init hello_init(void)
				{
					printk("============%s============\n",__func__);
					printk("a = %d\n",a);
					printk("str = %s\n",str);
					return 0;
				}
				
				module_param(a,int,0644);
				module_param(str,charp,0644);
			开发板中：装载时可传参
				[root@farsight /drv_module]# insmod hello_arg_drv.ko a=100 str=farsight
				============hello_init============
				a = 100
				str = farsight
			
			每个参数会在应用空间生成一个对应名称的文件:
				[root@farsight parameters]# ls /sys/module/hello_arg_drv/parameters/ -l
				total 0
				-rw-r--r--    1 0        0             4096 Jan  1 00:31 a
				-rw-r--r--    1 0        0             4096 Jan  1 00:31 str
		2> 模块调用
			被调用模块源代码：
				#include <linux/init.h>
				#include <linux/module.h>


				int my_add(int a,int b)
				{
					return a + b;  
				}
				EXPORT_SYMBOL(my_add);

				int my_sub(int a,int b)
				{
					return a - b;
				}
				EXPORT_SYMBOL(my_sub);

				MODULE_LICENSE("GPL");
			调用模块源代码：
				#include "fun.h"
				
				printk("a + b = %d\n",my_add(3,5));
				printk("a - b = %d\n",my_sub(3,5));

			开发板：
				
				[root@farsight /drv_module]# insmod fun.ko
				[root@farsight /drv_module]# insmod hello_fun_drv.ko
				============hello_init============
				a + b = 8
				a - b = -2



