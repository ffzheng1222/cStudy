
#ifndef __LED_DEV_PLAT_H_
#define __LED_DEV_PLAT_H_



#define GPC0_CON_BASE			0xE0200060	//led�ƿ��ƼĴ����Ļ���ַ
#define GPC0_SIZE				24			//led����6���Ĵ���
#define Con_reg_ADDR_OFFSET 	4			//led�����ݼĴ�����ַΪ----���ƼĴ�����ߵ�ַƫ�ƶ���




/*
	��pdrv�ж���
*/

//��������Զ��������
struct s5pv210_led_platdata{
	char *name ;			//�������ַ���
	int num; 				//����led��
	int shift ; 			//��λ��--��ʾgpio�ڵĺ���
	int conf_reg; 			// ���üĴ�����ֵ
	int conf_reg_mask;  	// ��������--���üĴ���
	int data_reg;  			// ���ݼĴ���������ֵ
	int data_reg_mask; 		// ��������--��ʱ�ò���
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
	void 	*reg_base;		// gpc0�Ĵ�������ʼλ��
	struct s5pv210_led_platdata *pdata;
};



#endif















