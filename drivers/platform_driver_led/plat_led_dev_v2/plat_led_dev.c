


#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


#include "led_dev_plat.h"




void led_dev_release(struct device *dev)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);
}



//ƽ̨�����������豸����Դ(��Ϣ)
struct platform_device led_pdev = {
	.name = "s5pv210_led",					//  ����ƥ��--/sys/bus/platform/devices/s5pv210_led(��·���ɼ�)
	.id	=	-1,								//һ��ֱ����-1---- ���ֲ�ͬ�Ŀ�����
	.dev =  {
		.release = led_dev_release,
	},
	.num_resources = ARRAY_SIZE(led_res),	//��Դ��Ŀ
	.resource = led_res,					//��Դ����---��������ַ���ж�
};




static int __init plat_led_dev_init(void)
{
	printk("--------^_^----%s-----\n", __FUNCTION__);

	//ע��pdev
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














