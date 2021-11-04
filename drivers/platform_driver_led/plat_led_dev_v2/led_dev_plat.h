
#ifndef __LED_DEV_PLAT_H_
#define __LED_DEV_PLAT_H_



//平台总线存放(操作逻辑部分) 与 (硬件信息部分) 匹配条件
struct platform_device_id  led_id_table[] = {
	{"s3c2440_led", 0x4444},
	{"s5pv210_led", 0x5555},
	{"exynos_4412", 0x4412},
};


//设计一个全局的设备对象
struct s5pv210_plat_led{
	dev_t devno;
	struct cdev *cdev;
	struct class *cls;
	struct device *dev;
	int irqno;
	struct resource *mem_res;	//存放从pdev中取出的资源的容器
								/*******************作用: 获取pdev中的数据***************/
	void * reg_base;		// gpc0寄存器的起始位置
};







#define GPC0_CON_BASE			0xE0200060	//led灯控制寄存器的基地址
#define GPC0_SIZE				24			//led灯有6个寄存器
#define Con_reg_ADDR_OFFSET 	4			//led灯数据寄存器地址为----控制寄存器向高地址偏移多少


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






#endif















