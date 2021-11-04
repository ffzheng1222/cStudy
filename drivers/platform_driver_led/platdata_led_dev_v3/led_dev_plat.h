
#ifndef __LED_DEV_PLAT_H_
#define __LED_DEV_PLAT_H_



#define GPC0_CON_BASE			0xE0200060	//led灯控制寄存器的基地址
#define GPC0_SIZE				24			//led灯有6个寄存器
#define Con_reg_ADDR_OFFSET 	4			//led灯数据寄存器地址为----控制寄存器向高地址偏移多少




/*
	在pdrv中定义
*/

//自行设计自定义的数据
struct s5pv210_led_platdata{
	char *name ;			//描述的字符串
	int num; 				//几个led灯
	int shift ; 			//移位数--表示gpio口的号码
	int conf_reg; 			// 配置寄存器的值
	int conf_reg_mask;  	// 用于清零--配置寄存器
	int data_reg;  			// 数据寄存器中最终值
	int data_reg_mask; 		// 用于清零--暂时用不上
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
	void 	*reg_base;		// gpc0寄存器的起始位置
	struct s5pv210_led_platdata *pdata;
};



#endif















