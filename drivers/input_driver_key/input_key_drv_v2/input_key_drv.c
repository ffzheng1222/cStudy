



#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>

#include <mach/gpio.h>



struct input_dev * key_inputdev;
static int irqno;



irqreturn_t input_key_handler(int irqno, void *dev_id)
{
	int value;
	printk("-------%s-------\n", __FUNCTION__);

	//���жϴ����ʱ�����ݴ��ݸ��û�
	value = gpio_get_value(S5PV210_GPH0(1));

	if(value)
	{
		//����̧��
			//����3  0----̧��1----����
		printk("<kernel>-----KEY_DOWN is up...\n");

		input_event(key_inputdev, EV_KEY, KEY_DOWN, !!0);
		input_sync(key_inputdev);

	}else{
		//��������
		printk("<kernel>-----KEY_DOWN is pressed...\n");

		input_event(key_inputdev, EV_KEY, KEY_DOWN, !!1);
		input_sync(key_inputdev);
	}

	return IRQ_HANDLED;
}




static int __init input_key_dry_init(void)
{
	int ret;
	printk("-------%s-------\n", __FUNCTION__);
	
	/*
		1������һ��input device����
		2�� ��ʼ��input device
		3,	ע��input device
		4, Ӳ����ʼ�������һ�ȡ������
	*/

	//1.����һ��input device����
	key_inputdev = input_allocate_device();
	if(IS_ERR(key_inputdev))
	{
		printk(KERN_ERR "input_allocate_device erron!\n");
		return -EFAULT;
	}

	//2.��ʼ��input device---��1---��1��λ��ĳ��λ����

		//��ҪΪ�û��ṩһЩ�鿴��Ϣ---/sys/class/input/eventX/devices/name...
	key_inputdev->name = "s5pv210_input_keys-1";
	key_inputdev->phys = "linuxkey/input/input0";
	key_inputdev->uniq = "0xFA00";
	key_inputdev->id.bustype = BUS_HOST;
	key_inputdev->id.vendor = 0x2323;
	key_inputdev->id.product = 0x6666;
	key_inputdev->id.version= 0x0001;
	
		//evbit��ʾ�豸�ܹ�������Щ��������
		//����input device�ܹ�������������
	__set_bit(EV_KEY, key_inputdev->evbit);

		//����input device�ܹ�������������ĸ�����
	__set_bit(KEY_DOWN, key_inputdev->keybit);
	__set_bit(KEY_UP, key_inputdev->keybit);



	//3,ע��input device
	ret = input_register_device(key_inputdev);
	if(ret != 0)
	{
		printk(KERN_ERR "input_register_device erron!\n");
		ret = -EINVAL;
		goto out1;
	}


	//4,Ӳ����ʼ�������һ�ȡ������(ͨ���ж�)
	irqno = IRQ_EINT(1);
	ret = request_irq(irqno, input_key_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "eint1_key", NULL);
	if(ret != 0)
	{
		printk(KERN_ERR "request_irq erron!\n");
		ret = -EINVAL;
		goto out2;
	}

	
	return 0;

out2:
	input_unregister_device(key_inputdev);
out1:
	input_free_device(key_inputdev);
	
	return ret;
}

static void __exit input_key_dry_exit(void)
{
	printk("-------%s-------\n", __FUNCTION__);

	free_irq(irqno, NULL);

	input_unregister_device(key_inputdev);

	input_free_device(key_inputdev);
}

 
module_init(input_key_dry_init);
module_exit(input_key_dry_exit);

MODULE_LICENSE("GPL");
















































