

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/i2c.h>

#include <plat/irqs.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/ioctl.h>



//创建i2c_e2prom设备描述对象
struct i2c_e2prom {
	unsigned int dev_major;
	struct device dev;
	struct i2c_client *client;		//用于记录匹配之后的client
};


struct i2c_e2prom *at24c02_i2c;


struct class at24_class = {
	.name = "at24_cls",
};



//自定义一个类似i2c_master_recv的函数接口
int at24c02_e2prom_i2c_read(const struct i2c_client *client, char *buf, int count)
{
	int ret;
	printk("--------%s------\n", __func__);
	
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = I2C_M_RD;
	msg.buf = buf;
	msg.len = count;
	

	ret = i2c_transfer(adapter, &msg, 1);

	return ret==1?count:ret;
}

//自定义一个类似i2c_master_send的函数接口
int at24c02_e2prom_i2c_write(const struct i2c_client *client, char *buf, int count)
{
	int ret;
	printk("--------%s------\n", __func__);
	
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg;

	msg.addr = client->addr;
	msg.flags = 0;
	msg.buf = buf;
	msg.len = count;
	

	ret = i2c_transfer(adapter, &msg, 1);

	return ret==1?count:ret;

}





ssize_t at24_drv_read(struct file *filp, char __user *buf, size_t count, loff_t *fops)
{
	int ret;
	printk("--------%s------\n", __func__);

	/*
		此处count代表用户给内核一个装数据的容器
		(因为存储设备内存地址有限，故需要限制容器的大小)		
	*/
	if(count < 0 || count > 256)
		return -ENOMEM;
	
	//需要动态分配空间-------因为不知道硬件存储设备中具体有多少数据，故需动态申请
	char *temp = kzalloc(count, GFP_KERNEL);	
	if(!temp)
	{
		printk(KERN_ERR "kzalloc erron!\n");
		return -ENOMEM;
	}

	//1.从硬件中读取数据
	ret = at24c02_e2prom_i2c_read(at24c02_i2c->client, temp, count);
	if(ret < 0)
	{
		printk(KERN_ERR "at24c02_e2prom_i2c_read erron!\n");
		kfree(temp);
		return -EINVAL;
	}
	

	//2.将从硬件中获取的数据拷贝给用户
	ret = copy_to_user(buf, temp, count);
	if(ret > 0)
	{
		printk(KERN_ERR "copy_to_user erron!\n");
		kfree(temp);
		return -EINVAL;
	}

	kfree(temp);

	return count;
}

ssize_t at24_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fops)
{
	int ret;
	printk("--------%s------\n", __func__);

	/*
		此处count代表一个内核的容器----用来存放用户传下来的数据
		(因为存储设备内存地址有限，故需要限制容器的大小)		
	*/
	if(count < 0 || count > 256)
		return -ENOMEM;

	
	//需要动态分配空间-------因为不知道硬件存储设备中具体有多少数据，故需动态申请
	char *temp = kzalloc(count, GFP_KERNEL);	
	if(!temp)
	{
		printk(KERN_ERR "kzalloc erron!\n");
		return -ENOMEM;
	}


	//1.从用户空间获取数据
	ret = copy_from_user(temp, buf, count);
	if(ret > 0)
	{
		printk(KERN_ERR "copy_from_user erron!\n");
		kfree(temp);
		return -EINVAL;
	}


	//2.将数据写入硬件
	ret = at24c02_e2prom_i2c_write(at24c02_i2c->client, temp, count);
	if(ret < 0)
	{
		printk(KERN_ERR "at24c02_e2prom_i2c_write erron!\n");
		kfree(temp);
		return -EINVAL;
	}
	
	kfree(temp);

	return count;
}




int at24_drv_open(struct inode *inode, struct file *filp)
{
	printk("--------%s------\n", __func__);

	return 0;
}

int at24_drv_close(struct inode *inode, struct file *filp)
{
	printk("--------%s------\n", __func__);


	return 0;
}




struct file_operations at24_i2c_fops = {
	.open = at24_drv_open,
	.release = at24_drv_close,
	.read = at24_drv_read,
	.write = at24_drv_write,
};




static void at24_dev_free(struct device *dev)
{

}


int at24_i2c_probe(struct i2c_client *client, const struct i2c_device_id *at24_id)
{
	int ret;
	printk("--------%s------\n", __func__);
	
	//最终目标是: 要和用户进行数据交互

	at24c02_i2c = kzalloc(sizeof(struct i2c_e2prom), GFP_KERNEL);
	if(IS_ERR(at24c02_i2c))
	{
		printk(KERN_ERR "kzalloc erron!\n");
		return -ENOMEM;
	}

	//1.申请主设备号
	at24c02_i2c->dev_major = register_chrdev(0, "at24c02_drv", &at24_i2c_fops);
	if(at24c02_i2c->dev_major < 0)
	{
		printk(KERN_ERR "kzalloc erron!\n");
		return -EINVAL;
		goto out1;
	}

	//2.记录client
	at24c02_i2c->client = client;

	//3.创建设备节点
			//3.1创建类
	ret = class_register(&at24_class);
	if(ret)
	{
		pr_err("unable to register input_dev class\n");
		goto out2;
	}
	

			//3.2创建设备节点
	dev_set_name(&at24c02_i2c->dev, "at24_dev");
			
	at24c02_i2c->dev.devt = MKDEV(at24c02_i2c->dev_major, 0);
	at24c02_i2c->dev.class = &at24_class;
	at24c02_i2c->dev.parent = &client->dev;
	at24c02_i2c->dev.release = at24_dev_free;
	device_initialize(&at24c02_i2c->dev);
	
	ret = device_add(&at24c02_i2c->dev);
	if(ret)
	{
		goto out3;
	}

	
	//4.初始化硬件----因为e2prom比较简单，不需要初始化

	return 0;

out3:
	class_unregister(&at24_class);
out2:
	unregister_chrdev(0, "at24c02_drv");
out1:
	kfree(at24c02_i2c);

	return ret;
}

int at24_i2c_remove(struct i2c_client *client)
{
	printk("--------%s------\n", __func__);

	
	device_del(&at24c02_i2c->dev);
	class_unregister(&at24_class);
	unregister_chrdev(0, "at24c02_drv");
	kfree(at24c02_i2c);
	
	return 0;
}


struct i2c_device_id at24_i2c_id_table[] = {
	{"at24c02", 0x04},
	{"at24c02", 0x08},
	{"at24c02", 0x10},
};
	

struct i2c_driver at24_i2c_pdrv = {
	.probe = at24_i2c_probe,		
	.remove = at24_i2c_remove,
	.driver = {
		.name = "at24c02_i2c_drv",
	},
	.id_table = at24_i2c_id_table,
};




static int __init at24c02_i2c_drv_init(void)
{
	printk("--------%s------\n", __func__);

	return i2c_add_driver(&at24_i2c_pdrv);
}

static void __exit at24c02_i2c_drv_exit(void)
{
	printk("--------%s------\n", __func__);

	i2c_del_driver(&at24_i2c_pdrv);
}



module_init(at24c02_i2c_drv_init);
module_exit(at24c02_i2c_drv_exit);

MODULE_LICENSE("GPL");
































