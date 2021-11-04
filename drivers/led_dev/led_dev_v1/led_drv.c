

#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/device.h>
//#include <linux/Kdev_t.h>
#include <linux/slab.h>


#include <linux/err.h>


#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/ioctl.h>


#define LED_NUM_ON              _IOW('L', 0X1111, int)
#define LED_NUM_OFF             _IOW('L', 0X2222, int)
#define LED_ALL_ON              _IO('L',  0X3333)
#define LED_ALL_OFF             _IO('L',  0X4444)





//定义一个设备的类
struct s5pv210_led{
    unsigned int led_major;     //主设备号
    struct class  *cls;         //设备类指针
    struct device *dev;         //设备节点指针
    int value;                  //用于得到内核中的一个切确的地址(copy_from_user函数的第一个参数)
};


//定义实例化对象的指针
struct s5pv210_led *led_dev;



volatile unsigned long * gpc0_con;
volatile unsigned long * gpc0_dat;




//设备操作为应用层提供的接口
                    /* 1.设备打开接口的实现 */
int led_open(struct inode *inode, struct file *file)
{
    printk("=====^_^=====%s=====\n", __func__);

    //设备初始化:设置输入
    *gpc0_con &= ~(0xff << 12);
    *gpc0_con |=  (0x11 << 12); //设置使能

    return 0;
}

                    /* 2.设备关闭接口的实现 */
int led_close(struct inode *inode, struct file *file)                 
{
    printk("=====^_^=====%s=====\n", __func__);

    //使LED灯关闭
    *gpc0_dat &= ~(0x3 << 3);

    return 0;
}

                    /* 3.设备使能接口的实现(应用层与内核层的数据交互) */
ssize_t led_write(struct file *file, const char __user *buf, size_t size, loff_t *flag)
{
    int ret;
    printk("=====^_^=====%s=====\n", __func__);

    ret = copy_from_user(&led_dev->value, buf, size);
    if(ret != 0)
    {
        printk(KERN_ERR "copy_from_user erron!\n");
        return -EFAULT;
    }
    
    if(led_dev->value)
    {
        //灯亮 
        *gpc0_dat |= 0x3 << 3;
    }
    else
    {
        //灯灭
        *gpc0_dat &= ~(0x3 << 3);
    }
    
    return size;

}


                    /* 4. 命令控制接口的实现(为用户空间提供更多的使用方法) */
long led_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int num = args;     //两个LED灯，实现分别操作
    printk("=====^_^=====%s=====\n", __func__);

    switch(cmd)
    {
        case LED_NUM_ON:
            if(num != 3 && num != 4)
            {
                return -EINVAL;
            }
            else
            {
                *gpc0_dat |= 0x1 << num;
            }
            printk("led-on: %d\n", num-2);
            break;
        case LED_NUM_OFF:
            if(num != 3 && num != 4)
            {
                return -EINVAL;
            }
            else
            {
                *gpc0_dat &= ~(0x1 << num);
            }
            printk("led-off: %d\n", num-2);
            break;
        case LED_ALL_ON:
            if(num != 3 && num != 4)
            {
                return -EINVAL;
            }
            else
            {
                *gpc0_dat |= 0x3 << 3;
            }
            printk("led-on: all\n");
            break;
        case LED_ALL_OFF:
            if(num != 3 && num != 4)
            {
                return -EINVAL;
            }
            else
            {
                *gpc0_dat &= ~(0x3 << 3);
            }
            printk("led-off: all\n");
            break;
        default:
            printk("unkown cmd\n");
            return -EINVAL;
    }
}


//设备的操作
struct file_operations file_oprat = {
    .open = led_open,
    .release = led_close,
    .write = led_write,
    .unlocked_ioctl = led_ioctl,
};


static int __init led_init(void)
{
    int ret;
    printk("=====^_^=====%s=====\n", __func__);

    //0.给设备申请空间
    led_dev = (struct s5pv210_led *)kzalloc(sizeof(struct s5pv210_led), GFP_KERNEL);
    if(led_dev == NULL)
    {
        printk(KERN_ERR "kzalloc erron!\n");
        return -ENOMEM;
    }

    
    
    //1.申请设备号
    led_dev->led_major = 300;
    ret = register_chrdev(led_dev->led_major, "led_drv", &file_oprat);
    if(ret < 0)
    {
        printk(KERN_ERR "register_chrdev erron!\n");
        ret = -EINVAL;
        goto err_free;
    }



    //2.创建设备节点
    led_dev->cls = class_create(THIS_MODULE, "led_cls");
    if(IS_ERR((void *)led_dev->cls))    //对返回的是指针类函数的出错处理(处理的方法之一)
    {
        printk(KERN_ERR "class_create erron!\n");
        ret = -EINVAL;
        goto err_chrdev;
    }

    led_dev->dev = device_create(led_dev->cls, NULL, MKDEV(led_dev->led_major, 0), NULL, "led");
    if(IS_ERR((void *)led_dev->dev))
    {
        printk(KERN_ERR "device_create erron!\n");
        ret = -EINVAL;
        goto err_class;
    }



    //3.物理地址映射或者中断申请
    gpc0_con = ioremap(0xE0200060, 8);
    if(IS_ERR((void *)gpc0_con))
    {
        printk(KERN_ERR "ioremap erron!\n");
        ret = -EINVAL;
        goto err_device;
    }
    gpc0_dat = gpc0_con + 1;    //此数据寄存器 与 控制寄存器 在物理地址上相差4个字节


    return 0;        /**************正常退出**************/

    
err_device:
    device_destroy(led_dev->cls, MKDEV(led_dev->led_major, 0));     //销毁创建的设备节点
err_class:
    class_destroy(led_dev->cls);                                    //销毁创建的具体设备类
err_chrdev:
    unregister_chrdev(led_dev->led_major, "led_drv");               //销毁申请的主设备号
err_free:
    kfree(led_dev);                                                 //释放给设备申请的空间


    return ret;      /**************异常退出**************/


}


static void __exit led_exit(void)
{
    printk("=====^_^=====%s=====\n", __func__);
    
    iounmap(gpc0_con);  //???????????????????????????????????不知道为什么   不带参的宏

    device_destroy(led_dev->cls, MKDEV(led_dev->led_major, 0));
    class_destroy(led_dev->cls);  
    unregister_chrdev(led_dev->led_major, "led_drv");  
    kfree(led_dev);
}



module_init(led_init);
module_exit(led_exit);



MODULE_LICENSE("GPL");

















