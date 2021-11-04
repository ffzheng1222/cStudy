

#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/device.h>
#include <linux/Kdev_t.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/icotl.h>




#define 	USE_STATIC_MAJOR	1
#define 	BUTTON_MAJOR		300


//设计按键数据包
struct button_event{
		int code;		//代表按键的名字		上下左右键......
		int value;		//按键的状态		按下/抬起-------1/0	
};


//定义一个设备的类
struct s5pv210_button{
	dev_t 				devno;
	struct class    	*cls;         	 //设备类指针
	struct device   	*dev;     	   	 //设备节点指针
	struct cdev     	*cdev;			 //设备的信息	
	int					irqno;			 //中断号
	struct button_event event;			 //按键的数据

	wait_queue_head_t	wq_head;		 //阻塞实现的等待队列头
	int			 have_data_fags;		 //资源是否到达的标志-----默认为0
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
    printk("=====^_^=====%s=====\n", __func__);

}





irqreturn_t  button_irq_handler(int irqno, void * dev_id)
{
	printk("=====^_^=====%s=====\n", __func__);
	
	int value = gpio_get_value(S5PV210_GPH0(1));

	if(value){
			//按键2 	抬起
		printk("<KERNEL>------button2 upspring...\n");
		button_dev->event.code  = KEY_DOWN;
		button_dev->event.value = 0; 
	}else{
			//按键2 	按下
		printk("<KERNEL>------button2 pressed...\n");
		button_dev->event.code  = KEY_DOWN;
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
		1. 申请设备号---register_chrdev_region
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
    button_dev->irqno = IRQ_EINT(1);
	ret = request_irq(button_dev->irqno, button_irq_handler, IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING, "button2 down", NULL);
	if(ret != 0)
	{
		printk(KERN_ERR "request_irq erron!\n");
		ret = -EBUSY;
		goto err_device;
	}


	//初始化阻塞实现的等待队列头
	init_waitqueue_head(&button_dev->wq_head);


    return 0;        /**************正常退出**************/

    
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
    
    free_irq(button_dev->irqno, NULL);
    device_destroy(button_dev->cls, button_dev->devno);
    class_destroy(button_dev->cls);
	cdev_del(button_dev->cdev);
    unregister_chrdev_region(button_dev->devno, 1);
    kfree(button_dev);
}



module_init(button_init);
module_exit(button_exit);



MODULE_LICENSE("GPL");

















