



#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>

#include <mach/gpio.h>



struct input_dev * key_inputdev;
static int irqno;



irqreturn_t input_key_handler(int irqno, void *dev_id)
{
	int value;
	printk("-------%s-------\n", __FUNCTION__);

	//在中断处理的时候将数据传递给用户
	value = gpio_get_value(S5PV210_GPH0(1));

	if(value)
	{
		//按键抬起
			//参数3  0----抬起，1----按下
		printk("<kernel>-----KEY_DOWN is up...\n");

		input_event(key_inputdev, EV_KEY, KEY_DOWN, !!0);
		input_sync(key_inputdev);

	}else{
		//按键按下
		printk("<kernel>-----KEY_DOWN is pressed...\n");

		input_event(key_inputdev, EV_KEY, KEY_DOWN, !!1);
		input_sync(key_inputdev);
	}

	return IRQ_HANDLED;
}




static int __init input_key_dry_init(void)
{
	int ret;
	printk("-------%s-------\n", __FUNCTION__);
	
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



	//3,注册input device
	ret = input_register_device(key_inputdev);
	if(ret != 0)
	{
		printk(KERN_ERR "input_register_device erron!\n");
		ret = -EINVAL;
		goto out1;
	}


	//4,硬件初始化，并且获取到数据(通过中断)
	irqno = IRQ_EINT(1);
	ret = request_irq(irqno, input_key_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "eint1_key", NULL);
	if(ret != 0)
	{
		printk(KERN_ERR "request_irq erron!\n");
		ret = -EINVAL;
		goto out2;
	}

	
	return 0;

out2:
	input_unregister_device(key_inputdev);
out1:
	input_free_device(key_inputdev);
	
	return ret;
}

static void __exit input_key_dry_exit(void)
{
	printk("-------%s-------\n", __FUNCTION__);

	free_irq(irqno, NULL);

	input_unregister_device(key_inputdev);

	input_free_device(key_inputdev);
}

 
module_init(input_key_dry_init);
module_exit(input_key_dry_exit);

MODULE_LICENSE("GPL");
















































