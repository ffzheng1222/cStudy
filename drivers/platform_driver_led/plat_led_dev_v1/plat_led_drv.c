



#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>


#include <asm/io.h>




static volatile unsigned long * gpc0_base;


//平台总线存放(操作逻辑部分) 与 (硬件信息部分) 匹配条件
struct platform_device_id  led_id_table[] = {
	{"s3c2440_led", 0x4444},
	{"s5pv210_led", 0x5555},
	{"exynos_4412", 0x4412},
};




int led_drv_probe(struct platform_device *pdev)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);	
	

	int ret;
	int irqno;
	
	/*******************作用: 获取pdev中的数据***************/
	struct resource *mem_res;	//存放从pdev中取出的资源的容器

	
	//1. 提供获取资源的接口
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(IS_ERR(mem_res))
	{
		printk(KERN_ERR "platform_get_resource erron!\n");
		ret = -ENOMEM;
		goto out1_resource;
	}
	
	//2. 如果是内存地址资源，需要进行映射
	gpc0_base = ioremap(mem_res->start, resource_size(mem_res));
	if(gpc0_base == NULL)
	{
		printk(KERN_ERR "ioremap erron!\n");
		ret = -EFAULT;
		goto out2_ioremap;
	}
	
	
	////2. 也可以获取中断资源
	irqno = platform_get_irq(pdev, 0);
	if(irqno == -ENXIO)
	{
		printk(KERN_ERR "platform_get_irq erron!\n");
		ret = -EINVAL;
		goto out3_irq;
	}
	printk("------irqno = 0x%x\n", irqno);
	
	

	return 0;


out3_irq:
	//???????????????????????????????????????????

out2_ioremap:
	iounmap(gpc0_base);

out1_resource:
	//???????????????????????????????????????????

	return ret;
	
}




int led_drv_remove(struct platform_device *pdev)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);
	

	//用于测试remove方法的调用过程
//	int *p = platform_get_drvdata(pdev);
//	printk("------p = 0x%x\n", *p);

	iounmap(gpc0_base);
	
	return 0;
}


/* 描述设备的操作方法 */
struct platform_driver led_pdrv = {
	.probe = led_drv_probe,
	.remove = led_drv_remove,
	
	.driver = {//一定要初始化
		.name = "samsung_led",		//自定义写---1， /sys/bus/platform/drivers/ samsung_led(路径可查看)
									//2,用于和pdev进行匹配，如果没有id_table再和.driver中的name匹配
	},
	.id_table = led_id_table, 		//id_table优先匹配和pdev中名字
};




static int __init plat_led_drv_init(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	return platform_driver_register(&led_pdrv);
}

static void __exit plat_led_drv_exit(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	platform_driver_unregister(&led_pdrv);
}



module_init(plat_led_drv_init);
module_exit(plat_led_drv_exit);



MODULE_LICENSE("GPL");















