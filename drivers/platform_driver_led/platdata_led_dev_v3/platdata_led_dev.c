


#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


#include "led_dev_plat.h"


struct s5pv210_led_platdata  led_platdata = {
	.name = "gpc0_3_4",
	.num = 2,
	.shift = 3,
	.conf_reg = 0x11,
	.conf_reg_mask = 0xff,
	.data_reg = 0x3,
	.data_reg_mask = 0x3,
};


//s5pv210开发板资源信息
struct resource led_res[] = {
	[0] = {
		.start = GPC0_CON_BASE,
		.end = 	 GPC0_CON_BASE + Con_reg_ADDR_OFFSET - 1,
		.name =  "samsung_led",
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = 0X1111,
		.end =   0X1111,
		.name =  "test_irq",
		.flags = IORESOURCE_IRQ,
	},
};







void led_dev_release(struct device *dev)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);
}



//平台总线中描述设备的资源(信息)
struct platform_device led_pdev = {
	.name = "s5pv210_led",					//  用于匹配--/sys/bus/platform/devices/s5pv210_led(此路径可见)
	.id	=	-1,								//一般直接填-1---- 区分不同的控制器
	.dev =  {
		.release = led_dev_release,
		.platform_data = &led_platdata,
	},
	.num_resources = ARRAY_SIZE(led_res),	//资源数目
	.resource = led_res,					//资源对象---可以填充地址和中断
};




static int __init platdata_led_dev_init(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	//注册pdev
	return platform_device_register(&led_pdev);
}



static void __exit platdata_led_dev_exit(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	platform_device_unregister(&led_pdev);
}



module_init(platdata_led_dev_init);
module_exit(platdata_led_dev_exit);


MODULE_LICENSE("GPL");















