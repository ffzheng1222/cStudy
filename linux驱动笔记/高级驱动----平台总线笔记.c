


1， 总线中的三个元素：
	通用三元素：
				/* 总线--完成匹配 */
				struct bus_type; 
					struct bus_type {
						const char		*name;  	// /sys/bus/路径下可以看到
						int (*match)(struct device *dev, struct device_driver *drv);
				}
				
	
		/ ***********************************父类通用************************************/
				/* 描述设备的信息 */
				struct device; 
					struct device {
						const char		*init_name; 
						struct bus_type	*bus;	 	//用于指定注册到哪个总线
					}
			
				/* 描述设备的操作方法 */
				struct device_driver; 
					struct device_driver {
						const char		*name;
						struct bus_type		*bus;
						
						int (*probe) (struct device *dev);
						int (*remove) (struct device *dev);
					}
	
	 平台总线的三元素：
			// platform_bus_type是在开机的时候构建
			//--bus_register(&platform_bus_type);
			struct bus_type ;
				struct bus_type platform_bus_type = {
					.name		= "platform",
					.dev_attrs	= platform_dev_attrs,
					.match		= platform_match,		//用于pdev 和 pdrv 之间相互匹配的
					.uevent		= platform_uevent,
					.pm		= &platform_dev_pm_ops,
				};
				
				
			//描述设备资源(信息)	
			struct platform_device;	 
				struct platform_device {
					const char	* name;		  		//用于匹配
					int		id; 			 		// 一般直接填-1---- 区分不同的控制器
					struct device	dev;	 		// 父类
					u32		num_resources;			//资源个数
					struct resource	* resource;		// 资源对象---可以填充地址和中断
				};
				struct resource {
					resource_size_t start;			//若为内存地址-----则：为资源的首地址
														//若为中断-----则：为中断地址-----start/end地址相同
					resource_size_t end;			//若为内存地址-----则：为资源的末地址
														//若为中断-----则：为中断地址-----start/end地址相同
					
					const char *name; 				//自定义
					unsigned long flags; 			//区分资源是地址还是中断
				};
				
		
			//描述设备的操作方法
			struct platform_driver;
			struct platform_driver {
				int (*probe)(struct platform_device *);			//在匹配成功之后被调用
				int (*remove)(struct platform_device *);		//pdev移除总线之后被调用
				struct device_driver driver;					// 父类---用于注册
				const struct platform_device_id *id_table;		//匹配表格
			};
			
			

编写代码：
	pdrv.c ：实现操作硬件的方法/逻辑
			 获取到pdev中的信息
	
	pdev.c : 定义资源信息
			
			
			
操作内存地址的方法：
		1，直接操作
			*gpc0_conf &= ~(0xff<<12);  //置零
			*gpc0_conf |= (0x11 << 12); //设置输出

		2，通过库函数
		
		
平台自定义数据：
	pdev：
			struct s5pv210_led_platdata  led_platdata = {
				.name = "gpc0_3_4",
				.num = 2,
				.shift = 3,
			};
			struct platform_device  led_pdev = {
				.dev = {
					.release = led_dev_release,	
					.platform_data = &led_platdata,
				},
			};
		
	pdrv中如何获取自定义数据：
		probe(*pdev)
			|
			pdev->dev.platform_data;
			
			
			
平台总线编程相关的函数接口
			/ ************************************************************************
			**	函 数 名：platform_device_register
			**	函数功能：注册pdev
			**  参数说明：参数1：描述设备资源(信息)的具体对象
			**	返 回 值：成功：返回	0
			**			  失败：返回	错误码
			**	头 文 件：#include <linux/platform_device.h>
			*************************************************************************/	
		int platform_device_register(struct platform_device *pdev)
		
			/ ************************************************************************
			**	函 数 名：platform_device_unregister
			**	函数功能：销毁pdev
			**  参数说明：参数1：描述设备资源(信息)的具体对象
			**	返 回 值：无
			**	头 文 件：#include <linux/platform_device.h>
			*************************************************************************/	
		void platform_device_unregister(struct platform_device *pdev)
		
		
		
			/ ************************************************************************
			**	函 数 名：platform_get_resource
			**	函数功能：通过pdev拿到资源(地址)
			**  参数说明：参数1：描述设备资源(信息)的具体对象
			**			  参数2：struct resource对象中资源具体是什么(地址/中断)-----IORESOURCE_MEM/IORESOURCE_IRQ
			**			  参数3：struct resource对象中同类资源的个数
			**	返 回 值：成功：返回整个struct resource资源对象
			**			  失败：NULL
			**	头 文 件：#include <linux/platform_device.h>
			*************************************************************************/	
		struct resource *platform_get_resource(struct platform_device *dev, unsigned int type, unsigned int num)
			/ ************************************************************************
			**	函 数 名：platform_get_irq
			**	函数功能：通过pdev拿到资源(中断)
			**  参数说明：参数1：描述设备资源(信息)的具体对象
			**			  参数2：struct resource对象中同类资源的个数
			**	返 回 值：成功：返回：中断号
			**			  失败：返回：错误码-ENXIO
			**	头 文 件：#include <linux/platform_device.h>
			*************************************************************************/	
		int platform_get_irq(struct platform_device *dev, unsigned int num)
			



--------------------------------------------------------------------
如果需要自己构建一个bus：
	bus_register(struct bus_type  *bus);
	
	//注册一个纯粹的父类
	device_register(struct device *dev); //注册一个device
	driver_register(struct device_driver *drv); //注册一个driver


2--读内核代码的技巧，跟读总线中如何匹配，并且如何调用的driver中probe方法

	1, 看代码主线---看主要函数
	2，出错判断和变量定义，一般不看
	3，看不懂的不看
	4，学会找度娘或者谷歌
	5， 做笔记和总结
	6, 勇敢的去猜
	-----------------------------------------
	
	platform_driver_register(&led_pdrv);
		|
		//设定driver需要注册到平台总线中去
		drv->driver.bus = &platform_bus_type;
		// 如果pdrv设定了probe，那么就给父类的driver 赋值一个probe方法
		if (drv->probe)
			drv->driver.probe = platform_drv_probe;
		if (drv->remove)
			drv->driver.remove = platform_drv_remove;
		//注册到总线中去
		driver_register(&drv->driver);
			|
			other = driver_find(drv->name, drv->bus);
			bus_add_driver(drv); //增加到总线
						|
						// 得到要注册的总线
						bus = bus_get(drv->bus);
						
						//分配一个private对象---用于链表中管理的节点
						priv = kzalloc(sizeof(*priv), GFP_KERNEL);
						
						// 初始化private对象
						klist_init(&priv->klist_devices, NULL, NULL);
						// 关联了当前被注册的driver
						priv->driver = drv; 
						drv->p = priv;
						priv->kobj.kset = bus->p->drivers_kset;
						error = kobject_init_and_add(&priv->kobj, &driver_ktype, NULL,
										 "%s", drv->name);
										 
										 
						driver_attach(drv);
							|
							bus_for_each_dev(drv->bus, NULL, drv, __driver_attach);
									|
									while ((dev = next_device(&i)) && !error)
												error = fn(dev, data);
												//__driver_attach(dev, drv);
													|
													struct device_driver *drv = data;
													if (!driver_match_device(drv, dev))
															|
														 drv->bus->match ? drv->bus->match(dev, drv) : 1;
													
													driver_probe_device(drv, dev);
														|
														really_probe(dev, drv);
															|
															if (dev->bus->probe) {// 总线中的probe
																	ret = dev->bus->probe(dev);
															else if (drv->probe) {// 父类中的probe
																	ret = drv->probe(dev);
											
						//将private对象注册到driver链表中去
						klist_add_tail(&priv->knode_bus, &bus->p->klist_drivers);
						
						module_add_driver(drv->owner, drv);
			driver_add_groups(drv, drv->groups);


	
		先调用父类中的probe：
			drv->driver.probe = platform_drv_probe;
					|
					struct platform_driver *drv = to_platform_driver(_dev->driver);
					struct platform_device *dev = to_platform_device(_dev);

					return drv->probe(dev); //最终调用到了子类中的probe，也即是调用pdrv中probe方法
			
				

		
		总结：
			1，如果要注册pdrv，将pdrv中父类注册到总线中去
			2，当drv添加到总线链表中去后，需要遍历另外一个链表device链表的device
			3， 拿出所有的device和当前driver进行匹配，匹配是用总线中match方法来完成
				如果总线中没有match方法， driver和device是永远匹配
			4， 如果匹配成功，先调用总线中的probe方法，(总线中如果没有probe方法)再调用父类driver中probe方法
			5，在平台总线中，先调用父类driver中probe方法，最后调用pdrv中probe方法



平台总线用在哪里的？
	1， 用于soc的升级的
			2410 ----------->   s5pv210 ------------->4412
				gpio控制器
				i2c控制器
				uart控制
				
				控制器的操作逻辑实际都是一样： pdrv
					gpio --> 1,配置功能
							2，修改数据寄存器
						
					i2c控制器：
							1,配置时钟
							2，设定i2c协议的开始，停止，ack，数据
					uart：
						1，配置时钟
						2，配置8n1， 波特率
						3，写数据写入到发送数据寄存器中
				
				控制器的地址不一样---------pdev
				
	2， 什么时候用平台总线---只要驱动操作某个地址或者是中断就可以用平台总线

	3， 平台总线只是一个功能，将资源和操作方法分离
	

	
内核中自带的pdev---内核开机就有很多的pdev，从何而来：
	arch/arm/mach-s5pv210/mach-smdkv210.c
	
		static struct platform_device *smdkv210_devices[] __initdata  = {
				&s3c_device_adc,
				&s3c_device_cfcon,
				&s3c_device_fb,
				&s3c_device_hsmmc0,
				&s3c_device_hsmmc1,
				&s3c_device_hsmmc2,
				&s3c_device_hsmmc3,
				&s3c_device_i2c0,
				&s3c_device_i2c1,
				&s3c_device_i2c2,
		}
		smdkv210_machine_init(void)//开机的时候会自动被执行
			|
			platform_add_devices(smdkv210_devices, ARRAY_SIZE(smdkv210_devices));
				|
				for (i = 0; i < num; i++) {
					ret = platform_device_register(devs[i]);			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
		