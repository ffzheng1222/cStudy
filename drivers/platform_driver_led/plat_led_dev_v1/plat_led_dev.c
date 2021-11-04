


#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>




#define GPC0_CON_BASE			0xE0200060	//led灯控制寄存器的基地址
#define GPC0_SIZE				24			//led灯有6个寄存器
#define Con_reg_ADDR_OFFSET 	4			//led灯数据寄存器地址为----控制寄存器向高地址偏移多少


//s5pv210开发板资源信息
struct resource led_res[] = {
	[0] = {
		.start = GPC0_CON_BASE,
		.end = 	 GPC0_CON_BASE + Con_reg_ADDR_OFFSET - 1,
		.name =  "gpc0_resource",
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
	},
	.num_resources = ARRAY_SIZE(led_res),	//资源数目
	.resource = led_res,					//资源对象---可以填充地址和中断
};




static int __init plat_led_dev_init(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	//注册pdev
	return platform_device_register(&led_pdev);
}



static void __exit plat_led_dev_exit(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	platform_device_unregister(&led_pdev);
}



module_init(plat_led_dev_init);
module_exit(plat_led_dev_exit);


MODULE_LICENSE("GPL");















