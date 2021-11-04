课程内容：中断---按键驱动

1、Linux驱动开发相关的结构体: struct file_operations, struct cdev,  struct  inode , struct  file 
2、新的字符设备的设备号申请方法
3、中断申请
4、文件IO模型的实现：阻塞和非阻塞
=================================================================================================
一、Linux驱动开发相关的结构体
	
	
	1、描述进程执行过程中用open打开的一个文件的信息: 
		//在应用空间打开一个文件就会产生一个struct file结构体。(动态产生)
		struct file {
			struct path		f_path;						//open打开的文件路径
			const struct file_operations	*f_op;		//文件操作对象
			unsigned int 		f_flags;				//文件的可读可写/阻塞与非阻塞
			fmode_t			f_mode;						//文件权限
			loff_t			f_pos;						//文件偏移量
			void			*private_data; 		    	//万能指针:指向私有数据
			......
		}
		
	
	2、描述一个字符设备的信息(设备号+文件操作对象)，任何一个字符设备驱动中都有该对象
		struct cdev {
			struct kobject kobj;					//基类(父类)
			struct module *owner;					//当前所属模块
			const struct file_operations *ops;		//文件操作对象
			struct list_head list;					//链表
			dev_t dev;								//设备号
			unsigned int count;						
			......
		};
		
	3、描述文件系统中某个文件的属性：静态产生
		struct inode {
			umode_t			i_mode;
			uid_t			i_uid;
			gid_t			i_gid;
			dev_t			i_rdev;
			const struct file_operations	*i_fop;
			struct timespec		i_atime;
			struct timespec		i_mtime;
			struct timespec		i_ctime;
			......
		}
		
	4、描述字符设备驱动中的设备的操作方法
		struct file_operations led_filp = {
			.open = led_open,
			.write = led_write,
			.release = led_close,
			.unlocked_ioctl = led_ioctl,
			.......
		};
		
	5、上面4个结构体直接的关系
	
		应用空间：
				fd = open("/dev/xxx",O_RDWR);		
		--------------------------------------------------------------
		内核VFS：sys_open();
				1.创建结构体：struct file：f_op
				2.根据/dev/xxx的属性: 设备号，找文件系统中的struct inode 
				3.f_op->f_open将被赋值为i_fop->i_open;
				4.根据inode中的设备号，在内核链表中找对应的cdev
				5.将cdev->ops赋给file->f_op
				6.返回struct file所在空间的下标
		
				
		
				4.创建设备节点：struct inode  --------> /dev/xxx
				3.将cdev加入到内核链表中
				2.创建一个cdev：struct cdev (设备号+操作对象)
				1.注册一个设备号
		-----------------------------------------------------------
		驱动：insmod XXX.ko
		

	6、通过filp拿到主次设备号的方法
		struct inode *node = filp->f_path.dentry->d_inode;
		int major = imajor(node);
		int minor = iminor(node);
		
	7、多个函数之间实现数据共享
		<一个函数把数据存放进filp中>
			static int a = 222;
			filp->private_data = &a;
		
		<另一个函数把数据从filp中拿出>
			int *p = (int *)filp->private_data;


		
二、新的字符设备的设备号申请方法
	
	1、静态申请设备号
		/ ************************************************************************
		**	函 数 名：register_chrdev_region
		**	函数功能：仅仅申请一个设备号
		**  参数说明：参数1：设备号
		**			  参数2：设备的个数
		**			  参数3：(设备名)字符串(驱动的描述) ---> /proc/device(可查看)
		**	返 回 值：成功: 返回 0;	
		**			  失败: 返回 < 0
		**	头 文 件：#include <asm/uaccess.h>
		************************************************************************/
		int register_chrdev_region(dev_t from, unsigned count, const char *name)
		
	2、动态申请设备号
		/ ************************************************************************
		**	函 数 名：alloc_chrdev_region
		**	函数功能：仅仅申请(动态申请)一个设备号
		**  参数说明：参数1：存放设备号变量的地址
		**			  参数2：次设备号的起始标号
		**			  参数3：设备的个数
		**			  参数4：(设备名)字符串(驱动的描述) ---> /proc/device(可查看)
		**	返 回 值：成功: 返回 0	
		**			  失败: 返回 < 0
		**	头 文 件：#include <asm/uaccess.h>
		************************************************************************/
		int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count,const char *name)
		
	
		-------------------------------------------------
		创建cdev结构体的方法：
			/ ************************************************************************
			**	函 数 名：cdev_alloc
			**	函数功能：分配一个struct cdev 对象空间
			**  参数说明：无
			**	返 回 值：成功: 返回申请的对象空间(指针)
			**			  失败: 返回 NULL 
			**	头 文 件：#include <linux/cdev.h>
			************************************************************************/
			cdev_alloc(void)
			/ ************************************************************************
			**	函 数 名：cdev_init
			**	函数功能：初始化cdev中的fops
			**  参数说明：参数1：描述一个字符设备信息的指针
			**			  参数2：描述字符设备驱动中的设备操作方法的指针
			**	返 回 值：无
			**	头 文 件：#include <linux/cdev.h>
			************************************************************************/
			cdev_init(struct cdev * cdev, const struct file_operations * fops)
			/ ************************************************************************
			**	函 数 名：cdev_add
			**	函数功能：将当前cdev注册到系统中
			**  参数说明：参数1：描述一个字符设备信息的指针
			**			  参数2：设备号
			**			  参数3：设备的个数
			**	返 回 值：成功：返回给内核成功注册的信息
			**			  失败：返回给内核注册失败的信息
			**	头 文 件：#include <linux/cdev.h>
			************************************************************************/
			cdev_add(struct cdev * p, dev_t dev, unsigned count)
		-------------------------------------------------
		应用举例:		
		//1>分配一个struct cdev 对象空间
		led_dev->cdev = cdev_alloc(void);

		//2>初始化cdev中的fops
		cdev_init(led_dev->cdev, &led_filp);

		//3>将当前cdev注册到系统中
		cdev_add(led_dev->cdev, led_dev->led_d, 1);
		
		
		
3,申请中断:
			irq:
			// 中断号获取: IRQ_EINT(1) 或者 去找irqs.h
			flags:
				#define IRQF_TRIGGER_NONE		0x00000000 		//内部中断触发
				#define IRQF_TRIGGER_RISING		0x00000001		//上升沿触发
				#define IRQF_TRIGGER_FALLING	0x00000002		//下降沿触发
				#define IRQF_TRIGGER_HIGH		0x00000004		//高电平触发
				#define IRQF_TRIGGER_LOW		0x00000008		//低电平触发
			/ ************************************************************************
			**	函 数 名：request_irq
			**	函数功能：申请中断
			**  参数说明：参数1：中断号
			**			  参数2：中断的处理方法irqreturn_t (*irq_handler_t)(int, void *);
			**			  参数3：中断的触发方式
			**			  参数4：表示一个字符串--自定义--/proc/interrupts
			**			  参数5：参数给第二个参数的数据
			**	返 回 值：成功: 返回 	0	
			**			  失败: 返回 	非0
			**	头 文 件：#include <linux/interrupt.h>
			************************************************************************/
			static inline int __must_check request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev)
	
	
	中断处理函数:
			irqreturn_t  key_irq_svc(int irqno, void *dev_id)
			{
				
				return IRQ_HANDLED;		//注意!!!	 中断处理函数(中断服务程序)最后一定返回IRQ_HANDLED
			}

	
	
	中断资源释放函数
			/ ************************************************************************
			**	函 数 名：free_irq
			**	函数功能：释放中断资源(不释放，下次申请中断，中断处理函数会出错)
			**  参数说明：参数1：request_irq函数申请的中断号
			**			  参数2：和request_irq中最后一个参数保持一致(参数5)
			**	返 回 值：无
			**	头 文 件：#include <linux/interrupt.h>
			************************************************************************/
			void free_irq(unsigned int irq, void *dev_id)



	
	
	
	
实现阻塞: 默认情况,大部分的函数默认都是阻塞
		  scanf()-- fgets()
		  accept(); read/recv/recvfrom
		
		0---需要一个等待队列头
			typedef struct __wait_queue_head wait_queue_head_t;
			/ ************************************************************************
			**	带 参 宏：init_waitqueue_head
			**	宏 功 能：初始化等待队列头
			**  参数说明：参数1： 实现阻塞的等待队列头------一个指针
			**	返 回 值：无
			**	头 文 件：#include <linux/wait.h>
			************************************************************************/
			#define init_waitqueue_head(q)
				//init_waitqueue_head(struct wait_queue_head_t *q)
			
			
		1,根据条件可以让进程进入到休眠状态
			/ ************************************************************************
			**	带 参 宏：wait_event_interruptible
			**	宏 功 能：根据条件可以让进程进入到休眠状态
			**  参数说明：参数1：表示等待队列头------(注意：等待队列变量)
			**			  参数2：表示一个条件--如果为假,就在此休眠,如果为真,就不休眠
			**	返 回 值：无
			**	头 文 件：#include <linux/wait.h>
			**			  #include <linux/sched.h>
			************************************************************************/
			#define wait_event_interruptible(wq, condition)	
					//wait_event_interruptible(struct wait_queue_head_t wq, int condition)
			
		
		2, 资源可达的时候需要唤醒
			/ ************************************************************************
			**	带 参 宏：wake_up_interruptible
			**	宏 功 能：资源可达的时候需要(何以)唤醒
			**  参数说明：参数1：表示进程在休眠状态可以被中断唤醒
			**	返 回 值：无
			**	头 文 件：#include <linux/wait.h>
			************************************************************************/
			#define wake_up_interruptible(x)
				//wake_up_interruptible(wait_queue_head_t *q)
	
	
非阻塞:
		在应用中设定非阻塞模式:
		int fd = open("/dev/key0", O_RDWR|O_NONBLOCK)
		
		read() 有数据就得到数据,没有数据就得到一个出错码--EAGAIN;
		-----------------------------------------------------------
		驱动:
			xxx_read
			{
				区分阻塞还是非阻塞
				if((filp->f_flags & O_NONBLOCK) && !key_dev->have_data)
				{
					return -EAGAIN;
				}

			}		
		
		
		
		
		
		
		