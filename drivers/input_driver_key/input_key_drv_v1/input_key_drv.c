



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
		input_report_key(key_inputdev, KEY_DOWN, 0);
		input_sync(key_inputdev);

	}else{
		//��������
		printk("<kernel>-----KEY_DOWN is pressed...\n");
		input_report_key(key_inputdev, KEY_DOWN, 1);
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

	printk("*****1****\n");

	//1.����һ��input device����
	key_inputdev = input_allocate_device();
	if(IS_ERR(key_inputdev))
	{
		printk(KERN_ERR "input_allocate_device erron!\n");
		return -EFAULT;
	}

	printk("*****2****\n");
	//2.��ʼ��input device---��1---��1��λ��ĳ��λ����
		//evbit��ʾ�豸�ܹ�������Щ��������
		//����input device�ܹ�������������
	key_inputdev->evbit[0] = BIT_MASK(EV_KEY);

		//����input device�ܹ�������������ĸ�����
	key_inputdev->keybit[BIT_WORD(KEY_DOWN)] |= BIT_MASK(KEY_DOWN);	
	key_inputdev->keybit[BIT_WORD(KEY_UP)] |= BIT_MASK(KEY_UP);
	key_inputdev->keybit[BIT_WORD(KEY_LEFT)] |= BIT_MASK(KEY_LEFT);
	key_inputdev->keybit[BIT_WORD(KEY_RIGHT)] |= BIT_MASK(KEY_RIGHT);

	printk("*****3****\n");

	//3,ע��input device
	ret = input_register_device(key_inputdev);
	if(ret != 0)
	{
		printk(KERN_ERR "input_register_device erron!\n");
		ret = -EINVAL;
		goto out1;
	}

	printk("*****4****\n");

	//4,Ӳ����ʼ�������һ�ȡ������(ͨ���ж�)
	irqno = IRQ_EINT(1);
	ret = request_irq(irqno, input_key_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "eint1_key", NULL);
	if(ret != 0)
	{
		printk(KERN_ERR "request_irq erron!\n");
		ret = -EINVAL;
		goto out2;
	}
	printk("*****5****\n");

	
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
















































