



#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>

#include <mach/gpio.h>






//设计一个描述按键的对象	irqno,gpio,name,code,flags
struct key_desc{
	int 			irqno;		//按键触发的中断号
	int 			gpio;		//按键对应的GPIO口
	char 			*name;		//按键的名字
	int 			code;		//代表按键的名字	
	unsigned long 	flags;		//按键触发中断的方式
};



//定义一个描述所有按键的集合
struct key_desc key_set[] = {
	[0] = {
		.name = 	"key1 up",
		.irqno = 	IRQ_EINT(0),
		.gpio = 	S5PV210_GPH0(0),
		.code =  	KEY_UP,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[1] = {
		.name = 	"key2 down",
		.irqno = 	IRQ_EINT(1),
		.gpio = 	S5PV210_GPH0(1),
		.code =  	KEY_DOWN,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[2] = {
		.name = 	"key3 left",
		.irqno = 	IRQ_EINT(2),
		.gpio = 	S5PV210_GPH0(2),
		.code =  	KEY_LEFT,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[3] = {
		.name = 	"key4 right",
		.irqno = 	IRQ_EINT(3),
		.gpio = 	S5PV210_GPH0(3),
		.code =  	KEY_RIGHT,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[4] = {
		.name = 	"key5 enter",
		.irqno = 	IRQ_EINT(4),
		.gpio = 	S5PV210_GPH0(4),
		.code =  	KEY_ENTER,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[5] = {
		.name = 	"key6 esc",
		.irqno = 	IRQ_EINT(5),
		.gpio = 	S5PV210_GPH0(5),
		.code =  	KEY_ESC,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[6] = {
		.name = 	"key7 home",
		.irqno = 	IRQ_EINT(22),
		.gpio = 	S5PV210_GPH2(6),
		.code =  	KEY_HOME,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	[7] = {
		.name = 	"key8 power",
		.irqno = 	IRQ_EINT(23),
		.gpio = 	S5PV210_GPH2(7),
		.code =  	KEY_POWER,
		.flags = 	IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
	},
	
};



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




static int __init input_allkey_dry_init(void)
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
	for(i = 0; i < ARRAY_SIZE(key_set); i++)
	{
		irqno = key_set[i].irqno;
		flags = key_set[i].flags;
		name = key_set[i].name;
		
		ret = request_irq(irqno, input_key_handler, flags, name, &key_set[i]);
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

static void __exit input_allkey_dry_exit(void)
{
	printk("-------%s-------\n", __FUNCTION__);
	int i = 0;

	for(i = 0; i< ARRAY_SIZE(key_set); i++)
		free_irq(key_set[i].irqno, &key_set[i]);

	input_unregister_device(key_inputdev);

	input_free_device(key_inputdev);
}

 
module_init(input_allkey_dry_init);
module_exit(input_allkey_dry_exit);

MODULE_LICENSE("GPL");
















































