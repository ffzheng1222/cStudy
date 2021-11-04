



#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

#include <mach/gpio.h>


#include "input_plat_key.h"


//设计一个描述所有按键的集合
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




//实现私有数据对象
struct my_platdata_key my_key_data = {
	.my_key_num = ARRAY_SIZE(key_set),
	.pdesc = key_set,
}; 






//设计设备资源对象
struct platform_device  key_pdev = {
	.name = "my_input_plat_key",
	.id = -1,
	.dev = {
		.platform_data = &my_key_data,

	},
};


static int __init input_plat_key_dey_init(void)
{
	printk("-------%s-------\n", __FUNCTION__);

	return platform_device_register(&key_pdev);
}


static void __exit input_plat_key_dey_exit(void)
{
	printk("-------%s-------\n", __FUNCTION__);

	return platform_device_unregister(&key_pdev);
}

 
module_init(input_plat_key_dey_init);
module_exit(input_plat_key_dey_exit);

MODULE_LICENSE("GPL");












