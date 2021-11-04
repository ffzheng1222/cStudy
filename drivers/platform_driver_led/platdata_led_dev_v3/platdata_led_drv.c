



#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>



#include <asm/uaccess.h>
#include <asm/io.h>

#include "led_dev_plat.h"




//ʵ�������豸����
struct s5pv210_plat_led *led_dev;



ssize_t led_drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *fop)
{
	int ret; 
	int value;	//�����û��ռ䷢�͸��ں˵�����

	printk("--------^_^----%s-----\n", __FUNCTION__);


	int shift = led_dev->pdata->shift;
	int data_reg = led_dev->pdata->data_reg;



	ret = copy_from_user(&value, buf, count);
	if(ret > 0)
	{
		printk(KERN_ERR "copy_from_user erron!\n");
		return -EINVAL;
	}

	
	if(value){
		//����
		writel(readl(led_dev->reg_base +4) | (data_reg << shift), led_dev->reg_base +4);

	}else{
		//���
		writel(readl(led_dev->reg_base +4) & ~(data_reg << shift), led_dev->reg_base +4);			
	}

	return count;
}

int led_drv_open(struct inode *inode, struct file *filp)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);	

	return 0;
}

int led_drv_close(struct inode *inode, struct file *filp)
{
	
	int shift = led_dev->pdata->shift;
	int data_reg = led_dev->pdata->data_reg;

	printk("--------^_^----%s-----\n", __FUNCTION__);	
	
	writel(readl(led_dev->reg_base + 4) & ~(data_reg << shift), led_dev->reg_base + 4);
	
	return 0;
}

struct file_operations  led_fops = {
	.open = led_drv_open,
	.release = led_drv_close,
	.write = led_drv_write,
};




int led_drv_probe(struct platform_device *pdev)
{
	int ret;
	printk("--------^_^----%s-----\n", __FUNCTION__);	

	//1. ʵ����ȫ�ֵ��豸����
	led_dev = kzalloc(sizeof(struct s5pv210_plat_led), GFP_KERNEL);
	if(IS_ERR(led_dev))
	{
		printk(KERN_ERR "kzalloc erron!\n");
		return	-ENOMEM;
	}


	// ��ȡ�Զ�������
	led_dev->pdata = pdev->dev.platform_data;


	//2.1 ��̬�����豸��
	ret = alloc_chrdev_region(&led_dev->devno, 0, 1, "plat_led_drv");
	if(ret != 0)
	{
		printk(KERN_ERR "alloc_chrdev_region erron!\n");
		ret = -EINVAL;
		goto out1_free;
	}


	//2.2 ����cdev
	led_dev->cdev = cdev_alloc();
	if(IS_ERR(led_dev->cdev))
	{
		printk(KERN_ERR "cdev_alloc erron!\n");
		ret = -EBUSY;
		goto out2_chrdev_region;
	}
	
	cdev_init(led_dev->cdev, &led_fops);

	ret = cdev_add(led_dev->cdev, led_dev->devno, 1);
	if(ret != 0)
	{
		printk(KERN_ERR "cdev_add erron!\n");
		ret = -ENOMEM;
		goto out3_cdev_alloc;
	}


	//3. ���������豸��
	led_dev->cls = class_create(THIS_MODULE, "led_cls");
	if(IS_ERR(led_dev->cls))
	{
		printk(KERN_ERR "class_create erron!\n");
		ret = -ENOMEM;
		goto out3_cdev_alloc;
	}

	//4. �����豸�ڵ�
	led_dev->dev = device_create(led_dev->cls, NULL, led_dev->devno, NULL, "led%d", 0);
	if(IS_ERR(led_dev->dev))
	{
		printk(KERN_ERR "device_create erron!\n");
		ret = -EFAULT;
		goto out4_class;
	}

	


	/********************5. ͨ��platform_busƽ̨���߳�ʼ��Ӳ��**********************/
	//5.1 �ṩ��ȡ��Դ�Ľӿ�
	led_dev->mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(IS_ERR(led_dev->mem_res))
	{
		printk(KERN_ERR "platform_get_resource erron!\n");
		ret = -ENOMEM;
		goto out5_device;
	}



	//5.2 ������ڴ��ַ��Դ����Ҫ����ӳ��
	led_dev->reg_base = ioremap(led_dev->mem_res->start, resource_size(led_dev->mem_res));
	if(led_dev->reg_base == NULL)
	{
		printk(KERN_ERR "ioremap erron!\n");
		ret = -EFAULT;
		goto out6_platform;
	}


	//5.3ͨ��ioremap��ȡ����Դ��ĳ�ʼ��Ӳ���������
				//(readl/writel�������ں��ж�Ӳ����ַ�Ĳ���)

	
	
	int shift = led_dev->pdata->shift* 4;
	int conf_reg_mask = led_dev->pdata->conf_reg_mask;
	int conf_reg = led_dev->pdata->conf_reg;

	
	unsigned long temp = readl(led_dev->reg_base);
	temp &= ~(conf_reg_mask << shift);
	temp |=  (conf_reg  << shift);
	writel(temp, led_dev->reg_base);

	
	////5.4 Ҳ���Ի�ȡ�ж���Դ(�ɸ����û������ж��Ƿ���Ҫʵ��)
	led_dev->irqno = platform_get_irq(pdev, 0);
	if(led_dev->irqno == -ENXIO)
	{
		printk(KERN_ERR "platform_get_irq erron!\n");
		ret = -EINVAL;
		goto out7_ioremap;
	}
	printk("------irqno = 0x%x\n", led_dev->irqno);
	

	//��������֮��˽�����ݵĽ���
	static int a = 0x8888;
	platform_set_drvdata(pdev, &a);

	

	return 0;


out7_ioremap:
	iounmap(led_dev->reg_base);
out6_platform:
	//???????????????????????????????????????????
out5_device:
	device_destroy(led_dev->cls, led_dev->devno);
out4_class:
	class_destroy(led_dev->cls);
out3_cdev_alloc:
	cdev_del(led_dev->cdev);
out2_chrdev_region:
	unregister_chrdev_region(led_dev->devno, 1);
out1_free:
	kfree(led_dev);
	
	return ret;
	
}




int led_drv_remove(struct platform_device *pdev)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);
	

	//���ڲ���remove�����ĵ��ù���
	int *p = platform_get_drvdata(pdev);
	printk("------p = 0x%x\n", *p);

	iounmap(led_dev->reg_base);

	device_destroy(led_dev->cls, led_dev->devno);
	class_destroy(led_dev->cls);
	cdev_del(led_dev->cdev);
	unregister_chrdev_region(led_dev->devno, 1);
	kfree(led_dev);
	
	return 0;
}



//ƽ̨���ߴ��(�����߼�����) �� (Ӳ����Ϣ����) ƥ������
struct platform_device_id  led_id_table[] = {
	{"s3c2440_led", 0x4444},
	{"s5pv210_led", 0x5555},
	{"exynos_4412", 0x4412},
};

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




static int __init platdata_led_drv_init(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	return platform_driver_register(&led_pdrv);
}

static void __exit platdata_led_drv_exit(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	platform_driver_unregister(&led_pdrv);
}



module_init(platdata_led_drv_init);
module_exit(platdata_led_drv_exit);



MODULE_LICENSE("GPL");















