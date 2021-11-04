
#ifndef __LED_DEV_PLAT_H_
#define __LED_DEV_PLAT_H_



//ƽ̨���ߴ��(�����߼�����) �� (Ӳ����Ϣ����) ƥ������
struct platform_device_id  led_id_table[] = {
	{"s3c2440_led", 0x4444},
	{"s5pv210_led", 0x5555},
	{"exynos_4412", 0x4412},
};


//���һ��ȫ�ֵ��豸����
struct s5pv210_plat_led{
	dev_t devno;
	struct cdev *cdev;
	struct class *cls;
	struct device *dev;
	int irqno;
	struct resource *mem_res;	//��Ŵ�pdev��ȡ������Դ������
								/*******************����: ��ȡpdev�е�����***************/
	void * reg_base;		// gpc0�Ĵ�������ʼλ��
};







#define GPC0_CON_BASE			0xE0200060	//led�ƿ��ƼĴ����Ļ���ַ
#define GPC0_SIZE				24			//led����6���Ĵ���
#define Con_reg_ADDR_OFFSET 	4			//led�����ݼĴ�����ַΪ----���ƼĴ�����ߵ�ַƫ�ƶ���


//s5pv210��������Դ��Ϣ
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















