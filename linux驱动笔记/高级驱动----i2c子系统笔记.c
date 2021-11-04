1，i2c 协议
2，i2c驱动框架
3，i2c 驱动编程方法---e2prom
4, i2c驱动框架完整代码流程
----------------------------------------

	
i2c驱动框架

	应用层
	-----------------------------------
	i2c driver:从设备驱动层
		与用户进行交互
		只需要提供数据，不需要知道数据如何传输给硬件
		
	---------------------------------------
	i2c core层：
		维护了i2c bus
		完成i2c driver和 i2c client的匹配
		
		drivers/i2c/i2c-core.c
		
	---------------------------------
	i2c adpater层：
		初始化i2c控制器
		完成数据传输给从设备的细节，不知道具体数据
		drivers/i2c/busses/i2c-s3c2410.c
		
3，i2c 驱动编程方法
	
	1，确保i2c core和i2c adapter层都编译进内核：
			 make menuconfig
				Device Drivers  --->
					<*> I2C support  --->  // i2c-core.c
						I2C Hardware Bus support  --->
							 <*> S3C2410 I2C Driver 
							 
		make zImage -j2
	
		记得更新：cp -raf arch/arm/boot/zImage  /tftpboot/
	2, 确保--存在i2c_client<--------不是直接创建，而是间接创建
		只需要提供i2c client的信息即可
		vim arch/arm/mach-s5pv210/mach-smdkv210.c
			如果需要在内核中提供一个i2c_client，就在以下的数组中添加信息
			static struct i2c_board_info smdkv210_i2c_devs0[] __initdata = {
				{ I2C_BOARD_INFO("at24c02", 0x50), }, //  "24c08"--用于和i2c_driver进行匹配
													// 0x50表示从设备的地址
				{ I2C_BOARD_INFO("wm8580", 0x1b), },
			};

			static struct i2c_board_info smdkv210_i2c_devs1[] __initdata = {
				/* To Be Updated */
			};

			static struct i2c_board_info smdkv210_i2c_devs2[] __initdata = {
		
			};
			
			smdkv210_machine_init(void)
				|
				i2c_register_board_info(0, smdkv210_i2c_devs0,ARRAY_SIZE(smdkv210_i2c_devs0));
				i2c_register_board_info(1, smdkv210_i2c_devs1,ARRAY_SIZE(smdkv210_i2c_devs1));
				i2c_register_board_info(2, smdkv210_i2c_devs2,	ARRAY_SIZE(smdkv210_i2c_devs2));
		
		
	
	
			修改之后的现象：
				[root@farsight 0-0050]# cat name
				at24c02
				[root@farsight 0-0050]# pwd
				/sys/bus/i2c/devices/0-0050

	
	
	3，--编写从设备驱动--i2c driver
		struct i2c_driver at24_drv = {
			.probe = at24_i2c_probe,
			.remove = at24_i2c_remove,
			.driver = {
				.name = "at24_i2c_drv", //不用于匹配--/sys/bus/i2c/drivers/at24_i2c_drv
			},
			.id_table = at24_id_table, // 用于匹配---一定要有

		};

			
		i2c_add_driver(&at24_drv);
	
	
	
	

	
重要的数据结构和接口：
struct i2c_driver {
		/* Standard driver model interfaces */
	int (*probe)(struct i2c_client *, const struct i2c_device_id *);
	int (*remove)(struct i2c_client *);
	
	struct device_driver driver; // 父类
	const struct i2c_device_id *id_table; //用于匹配
}

i2c总线对象：
struct bus_type i2c_bus_type = {
	.name		= "i2c",
	.match		= i2c_device_match,
	.probe		= i2c_device_probe,
	.remove		= i2c_device_remove,
	.shutdown	= i2c_device_shutdown,
	.pm		= &i2c_device_pm_ops,
};

struct i2c_client { //描述了一个从设备的信息--地址和名字
	unsigned short addr;// 7bit从设备地址
	char name[I2C_NAME_SIZE];
	struct i2c_adapter *adapter;// 创建i2c client的那个adapter
	struct i2c_driver *driver;	/* and our access routines	*/
	struct device dev;		//继承的父类
};
struct i2c_adapter { //描述了一个i2c控制器
	const struct i2c_algorithm *algo; //算法--用于操作控制的寄存器
			|
			int (*master_xfer)(struct i2c_adapter *adap, struct i2c_msg *msgs,   int num);
	struct device dev;//继承的父类
	
	int nr; //编号
	char name[48];
}
struct i2c_msg { //统一的传输数据包
	__u16 addr;	 //传输给哪个从设备的
	__u16 flags; // 读写标志
	__u16 len;	// 传输数据的长度
	__u8 *buf;		/* pointer to msg data			*/
};	

	
传输数据的接口：
	i2c_master_send(const struct i2c_client * client,const char * buf,int count)
	i2c_master_recv(const struct i2c_client * client,char * buf,int count)
		|最终都调用了
		int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
				|
				adap->algo->master_xfer(adap, msgs, num);
	
		
==========================================================================================================================
												 跟读i2c代码(linux源码)

跟读代码：
1--------mach-s5pv210/mach-smdkv210.c


static struct i2c_board_info smdkv210_i2c_devs0[] __initdata = {
	{ I2C_BOARD_INFO("24c08", 0x50), 
		//.type = "24c08", 
		//.addr = (0x50)
	},  
	{ I2C_BOARD_INFO("wm8580", 0x1b), },
};
smdkv210_machine_init(void)
	|
	i2c_register_board_info(0, smdkv210_i2c_devs0,ARRAY_SIZE(smdkv210_i2c_devs0));
		|
		for (status = 0; len; len--, info++) {
		struct i2c_devinfo	*devinfo;
		//分配一个节点
		devinfo = kzalloc(sizeof(*devinfo), GFP_KERNEL);
		//节点记录了总线的编号--0,1,2
		devinfo->busnum = busnum;
		//将smdkv210_i2c_devs0数组每个成员都赋值给节点中去
		devinfo->board_info = *info;
		//将整个节点加入到链表中
		list_add_tail(&devinfo->list, &__i2c_board_list);
	}
	
	/*
		&s3c_device_i2c0,
		&s3c_device_i2c1,
		&s3c_device_i2c2,
	*/
	//注册了以上的三个pdev
	platform_add_devices(smdkv210_devices, 
		ARRAY_SIZE(smdkv210_devices));




2,


	---------------------------------------------------
	i2c_driver
	
	
	
	
	---------------------------------------------------
	i2c-core.c
		postcore_initcall(i2c_init);
		module_exit(i2c_exit);	

		
		struct bus_type i2c_bus_type = {
			.name		= "i2c",
			.match		= i2c_device_match,
			.probe		= i2c_device_probe,
			.remove		= i2c_device_remove,
			.shutdown	= i2c_device_shutdown,
			.pm		= &i2c_device_pm_ops,
		};
		
		
			bus_register(&i2c_bus_type);
	
	
	
	
	
	------------------------------------------------------
	i2c-s3c2410.c
	static int __init i2c_adap_s3c_init(void)
	{
		return platform_driver_register(&s3c24xx_i2c_driver);
	}
	subsys_initcall(i2c_adap_s3c_init);

	static struct platform_driver s3c24xx_i2c_driver = {//这就是pdrv
		.probe		= s3c24xx_i2c_probe, //如果匹配成功就会被调用
		.remove		= s3c24xx_i2c_remove,
		.id_table	= s3c24xx_driver_ids,
		.driver		= {
			.owner	= THIS_MODULE,
			.name	= "s3c-i2c",
			.pm	= S3C24XX_DEV_PM_OPS,
		},
	}


	
	

	s3c24xx_i2c_probe()---pdrv中的probe
		|
		1,获取到自定义数据
			pdata = pdev->dev.platform_data;
			
			// 分配一个全局的设备对象--构建了adapter
			i2c = kzalloc(sizeof(struct s3c24xx_i2c), GFP_KERNEL);
		2，获取到内存地址资源platform_get_resource()
			res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		3，ioremap();
			i2c->regs = ioremap(res->start, resource_size(res));
		4,获取到中断资源
			i2c->irq = ret = platform_get_irq(pdev, 0);
		5, request_irq()
			ret = request_irq(i2c->irq, s3c24xx_i2c_irq, IRQF_DISABLED,
			  dev_name(&pdev->dev), i2c);
		
		6,构建一个i2c_adapter对象
			// 分配一个全局的设备对象--构建了adapter
			i2c = kzalloc(sizeof(struct s3c24xx_i2c), GFP_KERNEL);
		7,初始化i2c_adapter对象
			// 初始化adapters
			strlcpy(i2c->adap.name, "s3c2410-i2c", sizeof(i2c->adap.name));
			i2c->adap.owner   = THIS_MODULE;
			// 初始化算法i2c_transfer调用的是s3c24xx_i2c_xfer
			i2c->adap.algo    = &s3c24xx_i2c_algorithm;
			i2c->adap.retries = 2;
			i2c->adap.class   = I2C_CLASS_HWMON | I2C_CLASS_SPD;
			i2c->tx_setup     = 50;
		8,注册i2c_adapter对象
			i2c->adap.nr = pdata->bus_num;

			ret = i2c_add_numbered_adapter(&i2c->adap);
				|
				i2c_register_adapter(adap);
					|
					// 将当前的adapter注册到i2c bus中，/sys/bus/i2c/devices/i2c-0
					dev_set_name(&adap->dev, "i2c-%d", adap->nr);
					adap->dev.bus = &i2c_bus_type;
					adap->dev.type = &i2c_adapter_type;
					res = device_register(&adap->dev);
					
					if (adap->nr < __i2c_first_dynamic_bus_num)
						i2c_scan_static_board_info(adap);
							|
							list_for_each_entry(devinfo, &__i2c_board_list, list)
								|
								if (devinfo->busnum == adapter->nr
										//构建i2c_client,传递进当前的adapter,和i2c_board_info
										&& !i2c_new_device(adapter,&devinfo->board_info)
											|
											//构建一个 i2c_client
											client = kzalloc(sizeof *client, GFP_KERNEL);
												//关联当前的adapter
											client->adapter = adap;

											//将i2c_board_info中所有的值赋值给i2c_client
											client->dev.platform_data = info->platform_data;
											client->flags = info->flags;
											//得到从设备地址
											client->addr = info->addr; 
											client->irq = info->irq;
											//得到名字
											strlcpy(client->name, info->type, sizeof(client->name));
		
											//将当前的i2c_client注册到总线中去
											///sys/bus/i2c/devices/0-xxxx(从设备地址)
											client->dev.parent = &client->adapter->dev;
											client->dev.bus = &i2c_bus_type;
											client->dev.type = &i2c_client_type;
											client->dev.of_node = info->of_node;

											dev_set_name(&client->dev, "%d-%04x", i2c_adapter_id(adap),
													 client->addr);
											status = device_register(&client->dev);		
		
		
		