

#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/Kdev_t.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/mm.h>


#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/icotl.h>




#define 	USE_STATIC_MAJOR	1
#define 	BUTTON_MAJOR		300



//为验证mmap而设计的cmd，用于iocntl的实现
#define 	BUTTON_IOC_GET_DATA		0x6666;


//为验证mmap而设计的对象，用于iocntl的实现
struct mem_data{
	char buf[128];
};



//设计一个按键数据包
struct button_event{
		int code;		//代表按键的名字		上下左右键......
		int value;		//按键的状态		按下/抬起-------1/0	
};

//设计一个描述按键的对象	irqno,gpio,name,code,flags
struct button_desc{
	int 			irqno;		//按键触发的中断号
	int 			gpio;		//按键对应的GPIO口
	char 			*name;		//按键的名字
	int 			code;		//代表按键的名字	
	unsigned long 	flags;		//按键触发中断的方式
};

//定义一个描述所有按键的集合
struct button_desc button_set[] = {
	[0] = {
		.name = 	"button1 up"
		.irqno = 	IRQ_EINT(0);
		.gpio = 	S5PV210_GPH0(0);
		.code =  	KEY_UP;
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	},
	[1] = {
		.name = 	"button2 down"
		.irqno = 	IRQ_EINT(1);
		.gpio = 	S5PV210_GPH0(1);
		.code =  	KEY_DOWN;
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	},
	[2] = {
		.name = 	"button3 left"
		.irqno = 	IRQ_EINT(2);
		.gpio = 	S5PV210_GPH0(2);
		.code =  	KEY_LEFT;
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	},
	[3] = {
		.name = 	"button4 right"
		.irqno = 	IRQ_EINT(3);
		.gpio = 	S5PV210_GPH0(3);
		.code =  	KEY_RIGHT;
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	},
	[4] = {
		.name = 	"button5 enter"
		.irqno = 	IRQ_EINT(4);
		.gpio = 	S5PV210_GPH0(4);
		.code =  	KEY_ENTER;
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	},
	[5] = {
		.name = 	"button6 esc"
		.irqno = 	IRQ_EINT(5);
		.gpio = 	S5PV210_GPH0(5);
		.code =  	KEY_ESC;
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING;
	},
	
};




//定义一个设备的类
struct s5pv210_button{
	dev_t 				devno;
	struct class    	*cls;         	 //设备类指针
	struct device   	*dev;     	   	 //设备节点指针
	struct cdev     	*cdev;			 //设备的信息	
//	int					irqno;			 //中断号
	struct button_event event;			 //按键的数据

	wait_queue_head_t	wq_head;		 //阻塞实现的等待队列头
	int			 have_data_fags;		 //资源是否到达的标志-----默认为0

	void *			virt_mem;			//用于mmap映射的空间
};



//定义实例化对象的指针
struct s5pv210_button *button_dev;
	



//设备的操作
struct file_operations buttton_filp = {
    .open = 			button_open,
    .release = 			button_close,
    .write =		    button_write,  
    .read =				button_read, 
    .unlocked_ioctl = 	button_ioctl,
    .poll = 			button_poll,
    .mmap = 			button_mmap;
};

//设备操作为应用层提供的接口
                    /* 1.设备打开接口的实现 */
int button_open(struct inode *inode, struct file *filp)
{
    printk("=====^_^=====%s=====\n", __func__);
	
	//初始化event对象
	memset(&button_dev->event, 0, sizeof(struct button_event));
	//默认没有资源-----即0
	button_dev->have_data_fags = 0;
   
    return 0;
}

                    /* 2.设备关闭接口的实现 */
int button_close(struct inode *inode, struct file *filp)                 
{
    printk("=====^_^=====%s=====\n", __func__);

    return 0;
}

                    /* 3.设备使能接口的实现(应用层与内核层的数据交互) */
ssize_t button_write(struct file *filp, const char __user *buf, size_t count, loff_t *fops)
{

	printk("=====^_^=====%s=====\n", __func__);

    return count;
}


					/* 4.设备使能接口的实现(内核层与用户层的数据交互) */
ssize_t button_read(struct file *filp, char __user *buf, size_t count, loff_t *fpos)
{
	int ret;
	printk("=====^_^=====%s=====\n", __func__);


	//区分阻塞与非阻塞
	if((filp->f_flags & O_NONBLOCK) && !button_dev->have_data_fags)
	{
		return -EAGAIN;
	}

	//判断是否有资源
	wait_event_interruptible(button_dev->wq_head, button_dev->have_data_fags);

	
	ret = copy_to_user(buf, &button_dev->event, count);
	if(ret > 0){
		printk(KERN_ERR "copy_to_user erron!\n");
		return -EFAULT;
	}

	//清空event
	memset(&button_dev->event, 0, sizeof(struct button_event));
	//拷贝完成后-----表示没有资源(数据)
	button_dev->have_data_fags = 0;
	
	return count;
}


                    /* 5. 命令控制接口的实现(为用户空间提供更多的使用方法) */
long button_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	int ret;
    printk("=====^_^=====%s=====\n", __func__);

	//将第三个参数转换指针
	void __user * pargs = (void __user *)args;

	struct mem_data data;

	switch(cmd)
	{
		case BUTTON_IOC_GET_DATA:

			//把mmap映射空间中的内容拷贝到data容器中
			memcpy(&data, button_dev->virt_mem, 128);
			
			ret = copy_to_user(pargs, &data, sizeof(struct mem_data));
			if(ret > 0)
			{
				printk(KERN_ERR "copy_to_user erron!\n");
				return -EFAULT;
			}
			break;
			
		default:
			printf("unknow cmd!!!\n");
	}


	return 0;
}
					
					/* 6. 多路复用poll的实现 */
unsigned int button_poll(struct file *filp, struct poll_table_struct *pts)
{
	printk("=====^_^=====%s=====\n", __func__);

	unsigned int mask = 0;

	//1.将当前等待队列头注册到vfs当中去
	poll_wait(filp, &button_dev->wq_head, pts);

	
	//2.如果有数据返回pollin，没有数据返回0
	 if(button_dev->have_data_fags)
	 {
	 	mask |= POLLIN;
	 }

	 return mask;
}

					
					/* 7. IO mmap的实现 */
int button_mmap(struct file *filp, struct vm_area_struct *vma)
{
	printk("=====^_^=====%s=====\n", __func__);

	unsigned  long addr;

	// 只要调用io_remap_pfn_range就能将驱动中内存映射到用户空间	
	//vma用于描述用户空间映射的需求, vma是vfs层传递过来


	addr = virt_to_phys(button_dev->virt_mem);


	vma->vm_flags |= VM_IO;	
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);	


	//参数1---表示需求	
	//参数2---映射到用户空间的起始位置	
	//参数3--被映射的物理地址的页地址
	//参数4---映射的大小
	//参数5--映射的权限	
	if (io_remap_pfn_range(vma, vma->vm_start, addr >> PAGE_SHIFT,	
		PAGE_SIZE, vma->vm_page_prot)) 
	{		
		printk(KERN_ERR "%s: io_remap_pfn_range failed\n",	
			__func__);	
		return -EAGAIN;	
	}

	return 0;
}




irqreturn_t  button_irq_handler(int irqno, void * dev_id)
{
	printk("=====^_^=====%s=====\n", __func__);

	struct button_desc *p = (struct button_desc *)dev_id;
	
	int value = gpio_get_value(p->gpio);

	if(value){
			//按键2 	抬起
		printk("<KERNEL>------%s upspring...\n", p->name);
		button_dev->event.code  = p->code;
		button_dev->event.value = 0; 
	}else{
			//按键2 	按下
		printk("<KERNEL>------%s pressed...\n", p->name);
		button_dev->event.code  = p->code;
		button_dev->event.value = 1; 
	}


	//有资源(数据)到达触发中断
	button_dev->have_data_fags = 1;
	//唤醒等待队列
	wake_up_interruptible(&button_dev->wq_head);

	return IRQ_HANDLED;
}


static int __init button_init(void)
{

	/*	编写驱动的套路	2	
		0. 实例化全局的设备对象-- kzalloc	
		1. 申请设备号---unregister_chrdev_alloc
		   1.1自行创建cdev 对象	cdev_alloc / cdev_init / cdev_add
		   
		2. 自动创建设备节点---class_create, device_create		
		3. 初始化硬件--		
		4. 实现 file_operation
	*/ 


	int ret;
    printk("=====^_^=====%s=====\n", __func__);

    //0.给设备申请空间
    button_dev = (struct s5pv210_button *)kzalloc(sizeof(struct s5pv210_button), GFP_KERNEL);
    if(IS_ERR(button_dev))
    {
        printk(KERN_ERR "kzalloc erron!\n");
        return -ENOMEM;
    }



	#ifndef		USE_STATIC_MAJOR
	//1.静态申请设备号

		button_dev->devno = MKDEV(BUTTON_MAJOR, 0);
	    ret = register_chrdev_region(button_dev->devno, 1, "button_new_dev");
	    if(ret < 0)
	    {
	        printk(KERN_ERR "register_chrdev_region erron!\n");
	        ret = -EINVAL;
	        goto err_free;
	    }

	#else
	//1. 动态申请设备号

		ret = alloc_chrdev_region(&button_dev->devno, 0, 1, "button_new_dev");
		if(ret < 0)
		{
			printk(KERN_ERR "alloc_chrdev_region erron!\n");
			ret = -EINVAL;
			goto err_free;
		}
	
	#endif


	//1.1 自行创建cdev 对象	cdev_alloc / cdev_init / cdev_add
	button_dev->cdev = cdev_alloc();
	cdev_init(button_dev->cdev, &buttton_filp);
	ret = cdev_add(button_dev->cdev, button_dev->devno, 1);
	if(ret == 0)
	{
		printk(KERN_ERR "cdev_add error!\n");
		ret = -EINVAL;
		goto err_chrdev_region;
	}





    //2.创建设备节点
    button_dev->cls = class_create(THIS_MODULE, "button_cls");
    if(IS_ERR(button_dev->cls))    //对返回的是指针类函数的出错处理(处理的方法之一)
    {
        printk(KERN_ERR "class_create erron!\n");
        ret = -EINVAL;
        goto err_cdev;
    }

    button_dev->dev = device_create(button_dev->cls, NULL, button_dev->devno, NULL, "button%d", 0);
    if(IS_ERR(button_dev->dev))
    {
        printk(KERN_ERR "device_create erron!\n");
        ret = -EINVAL;
        goto err_class;
    }



    //3.物理地址映射或者------中断申请
	int i = 0;
	int irqno;
	char *name;
	unsigned long flags;
	for(i = 0; i < ARRAY_SIZE(button_set); i++)
	{
		irqno = button_set[i].irqno; 
		flags = button_set[i].flags;
		name  = button_set[i].name;

		ret = request_irq(irqno, button_irq_handler, flags, name, &button_set[i]);
		if(ret != 0)
		{
			printk(KERN_ERR "request_irq erron!\n");
			ret = -EBUSY;
			goto err_device;
		}
	}


	//初始化阻塞实现的等待队列头
	init_waitqueue_head(&button_dev->wq_head);


	//分配一块内存空间
	button_dev->virt_mem = kzalloc(PAGE_SIZE, GFP_KERNEL);
	if(IS_ERR(button_dev->virt_mem))
	{
		printk(KERN_ERR "kzalloc erron\n");
		ret = -ENOMEM;
		goto err_irq;
	}



    return 0;        /**************正常退出**************/



err_irq:
	for(i = 0; i < ARRAY_SIZE(button_set); i++)
	{
    	free_irq(button_set[i]->irqno, &button_set[i]);			//释放中断资源
	}
	
err_device:
    device_destroy(button_dev->cls, button_dev->devno);     	//销毁创建的设备节点
err_class:
    class_destroy(button_dev->cls);                             //销毁创建的具体设备类
err_cdev:
	cdev_del(button_dev->cdev);
	
err_chrdev_region:
	unregister_chrdev_region(button_dev->devno, 1);				//销毁静态申请的设备号
	
err_free:
    kfree(button_dev);                                          //释放给设备申请的空间


    return ret;      /**************异常退出**************/


}


static void __exit button_exit(void)
{
    printk("=====^_^=====%s=====\n", __func__);
	
	kfree(button_dev->virt_mem);
	
	int i = 0;
	for(i = 0; i < ARRAY_SIZE(button_set); i++)
	{
    	free_irq(button_set[i]->irqno, &button_set[i]);
	}
	
    device_destroy(button_dev->cls, button_dev->devno);
    class_destroy(button_dev->cls);
	cdev_del(button_dev->cdev);
    unregister_chrdev_region(button_dev->devno, 1);
    kfree(button_dev);
}



module_init(button_init);
module_exit(button_exit);



MODULE_LICENSE("GPL");

















