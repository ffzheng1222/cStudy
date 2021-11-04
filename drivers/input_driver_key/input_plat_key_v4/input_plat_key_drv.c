



#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>




#include "input_plat_key.h"



struct input_dev * key_inputdev;




irqreturn_t input_key_handler(int irqno, void *dev_id)
{
	int value;
	printk("-------%s-------\n", __FUNCTION__);


	struct key_desc *p = (struct key_desc *)dev_id;
	

	//在中断处理的时候将数据传递给用户
	value = gpio_get_value(p->gpio);

	if(value)
	{
		//按键抬起
			//参数3  0----抬起，1----按下
		printk("<kernel>-----%s is up...\n", p->name);

		input_event(key_inputdev, EV_KEY, p->code, !!0);
		input_sync(key_inputdev);

	}else{
		//按键按下
		printk("<kernel>-----%s is pressed...\n", p->name);

		input_event(key_inputdev, EV_KEY, p->code, !!1);
		input_sync(key_inputdev);
	}

	return IRQ_HANDLED;
}



int key_drv_probe(struct platform_device *pdev)
{
	
	int ret;
	printk("-------%s-------\n", __FUNCTION__);

	//从pdev中拿到私有数据
	struct my_platdata_key *mpdata = pdev->dev.platform_data;

	int key_num = mpdata->my_key_num;
	struct key_desc *mpdesc = mpdata->pdesc;

	
	/*
		1，构建一个input device对象
		2， 初始化input device
		3,	注册input device
		4, 硬件初始化，并且获取到数据
	*/

	//1.构建一个input device对象
	key_inputdev = input_allocate_device();
	if(IS_ERR(key_inputdev))
	{
		printk(KERN_ERR "input_allocate_device erron!\n");
		return -EFAULT;
	}

	//2.初始化input device---置1---将1移位到某个位置上

		//需要为用户提供一些查看信息---/sys/class/input/eventX/devices/name...
	key_inputdev->name = "s5pv210_input_keys-1";
	key_inputdev->phys = "linuxkey/input/input0";
	key_inputdev->uniq = "0xFA00";
	key_inputdev->id.bustype = BUS_HOST;
	key_inputdev->id.vendor = 0x2323;
	key_inputdev->id.product = 0x6666;
	key_inputdev->id.version= 0x0001;
	
		//evbit表示设备能够产生哪些数据类型
		//设置input device能够产生按键数据
	__set_bit(EV_KEY, key_inputdev->evbit);

		//设置input device能够产生具体的是哪个按键
	__set_bit(KEY_DOWN, key_inputdev->keybit);
	__set_bit(KEY_UP, key_inputdev->keybit);
	__set_bit(KEY_LEFT, key_inputdev->keybit);
	__set_bit(KEY_RIGHT, key_inputdev->keybit);
	__set_bit(KEY_ENTER, key_inputdev->keybit);
	__set_bit(KEY_ESC, key_inputdev->keybit);
	__set_bit(KEY_HOME, key_inputdev->keybit);
	__set_bit(KEY_POWER, key_inputdev->keybit);


	//3,注册input device
	ret = input_register_device(key_inputdev);
	if(ret != 0)
	{
		printk(KERN_ERR "input_register_device erron!\n");
		ret = -EINVAL;
		goto out1;
	}


	//4,硬件初始化，并且获取到数据(通过中断)
	
	int i = 0;
	int irqno;
	int flags;
	char * name;
	for(i = 0; i < key_num; i++)
	{
		irqno = mpdesc[i].irqno;
		flags = mpdesc[i].flags;
		name = mpdesc[i].name;
		
		ret = request_irq(irqno, input_key_handler, flags, name, &mpdata->pdesc[i]);
		if(ret != 0)
		{
			printk(KERN_ERR "request_irq erron!\n");
			ret = -EINVAL;
			goto out2;
		}

	}


	return 0;

out2:
	input_unregister_device(key_inputdev);
out1:
	input_free_device(key_inputdev);
	
	return ret;
}

int key_drv_remove(struct platform_device *pdev)
{
	printk("-------%s-------\n", __FUNCTION__);

	//从pdev中拿到私有数据
	struct my_platdata_key *mpdata = pdev->dev.platform_data;

	int key_num = mpdata->my_key_num;
	struct key_desc *mpdesc = mpdata->pdesc;

	
	int i = 0;

	for(i = 0; i< key_num; i++)
		free_irq(mpdesc->irqno, &mpdata->pdesc[i]);

	input_unregister_device(key_inputdev);

	input_free_device(key_inputdev);


	return 0;
}


//设计操作逻辑对象	
struct platform_driver  key_drv = {
	.probe = key_drv_probe,
	.remove = key_drv_remove,
	.driver = {
		.name = "my_input_plat_key",

	},
};



static int __init input_plat_key_dry_init(void)
{
	printk("-------%s-------\n", __FUNCTION__);

	return platform_driver_register(&key_drv);
}


static void __exit input_plat_key_dry_exit(void)
{
	printk("-------%s-------\n", __FUNCTION__);

	return platform_driver_unregister(&key_drv);
}

 
module_init(input_plat_key_dry_init);
module_exit(input_plat_key_dry_exit);

MODULE_LICENSE("GPL");
















































