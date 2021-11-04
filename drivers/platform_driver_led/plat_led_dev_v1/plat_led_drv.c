



#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>


#include <asm/io.h>




static volatile unsigned long * gpc0_base;


//ƽ̨���ߴ��(�����߼�����) �� (Ӳ����Ϣ����) ƥ������
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
	
	/*******************����: ��ȡpdev�е�����***************/
	struct resource *mem_res;	//��Ŵ�pdev��ȡ������Դ������

	
	//1. �ṩ��ȡ��Դ�Ľӿ�
	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(IS_ERR(mem_res))
	{
		printk(KERN_ERR "platform_get_resource erron!\n");
		ret = -ENOMEM;
		goto out1_resource;
	}
	
	//2. ������ڴ��ַ��Դ����Ҫ����ӳ��
	gpc0_base = ioremap(mem_res->start, resource_size(mem_res));
	if(gpc0_base == NULL)
	{
		printk(KERN_ERR "ioremap erron!\n");
		ret = -EFAULT;
		goto out2_ioremap;
	}
	
	
	////2. Ҳ���Ի�ȡ�ж���Դ
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
	

	//���ڲ���remove�����ĵ��ù���
//	int *p = platform_get_drvdata(pdev);
//	printk("------p = 0x%x\n", *p);

	iounmap(gpc0_base);
	
	return 0;
}


/* �����豸�Ĳ������� */
struct platform_driver led_pdrv = {
	.probe = led_drv_probe,
	.remove = led_drv_remove,
	
	.driver = {//һ��Ҫ��ʼ��
		.name = "samsung_led",		//�Զ���д---1�� /sys/bus/platform/drivers/ samsung_led(·���ɲ鿴)
									//2,���ں�pdev����ƥ�䣬���û��id_table�ٺ�.driver�е�nameƥ��
	},
	.id_table = led_id_table, 		//id_table����ƥ���pdev������
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















