主要内容---LCD屏驱动--Framebuffer子系统
1，LCD屏的硬件连接
2，Framebuffer子系统的框架
3，LCD屏的移植方法--时序配置
4，应用程序控制LCD屏的方法---应用显示图片
5， Framebuffer子系统的代码执行流程
----------------------------------------------



Framebuffer子系统的框架


		应用空间
	--------------------------------
		drivers/video/fbmem.c核心层： 1，与用户交互，实现fops
			2，实现mmap, 但是不知道映射内存的地址和大小
			
	------------------------------------------
		drivers/video/s3c-fb.c  控制器层
			1,完成硬件初始化
			2，分配dma显存，但是不知道如何映射到用户空间
		
	---------------------------------------

设定自定义数据的两种方式
1，直接在pdev中给定
	struct platform_device smdkv210_dm9000 = {
		.dev		= {
			.platform_data	= &smdkv210_dm9000_platdata,
		},
	};

2，通过一个函数接口去给定
	// 参数中所指向的数据就变成的自定义数据
void __init s3c_fb_set_platdata(struct s3c_fb_platdata *pd)
{
	struct s3c_fb_platdata *npd;

	if (!pd) {
		printk(KERN_ERR "%s: no platform data\n", __func__);
		return;
	}
	// 内部空间分配，并且拷贝pd所指向的数据，返回分配空间的地址
	npd = kmemdup(pd, sizeof(struct s3c_fb_platdata), GFP_KERNEL);
	if (!npd)
		printk(KERN_ERR "%s: no memory for platform data\n", __func__);

	s3c_device_fb.dev.platform_data = npd;//返回分配空间的地址赋值到自定义数据
}

	

	
s3c-fb.c ------------------------  mach-smdkv210.c
	pdrv						pdev
	
	
pdev :mach-smdkv210.c
	&s3c_device_fb,
		|
		struct platform_device s3c_device_fb = {
			.name		  = "s3c-fb",
			.id		  = -1,
			.num_resources	  = ARRAY_SIZE(s3c_fb_resource),
			.resource	  = s3c_fb_resource,
			.dev.dma_mask	  = &s3c_device_fb.dev.coherent_dma_mask,
			.dev.coherent_dma_mask = 0xffffffffUL,
		};
		
		smdkv210_machine_init(void)
				|
			//smdkv210_lcd0_pdata就是lcd驱动的自定义数据
			s3c_fb_set_platdata(&smdkv210_lcd0_pdata);
			platform_add_devices(smdkv210_devices, ARRAY_SIZE(smdkv210_devices));	
					|
					s3c_device_fb;
	



==================================================================================================================	
自定义数据：--开发的时候，立足于此处


 一、时序了解：
		资料1：soc时序图
				//S5PV210_UM_REV1.1-----soc数据手册--第09章--1--1.3--1.3.8 可查看
		资料2：LCD时序图
				//soc芯片手册文件夹----AT070TN92_LCD 可查看
		资料3：Linux内核时序图
				/home/farsight/S5PV210/kernel/linux-3.0.8
						----->Documentation/fb/framebuffer.txt 
	
			帧同步时序(VSYNC)：
			soc              		lcd外设									驱动中
				VSPW					tvpw          1<y<20   y=10     vsync_len=10
				VBPD					tvb-tvpw      23-tvpw  13       upper_margin=13
				LINVAL					tvd			  480            	yres=480
				VFPD					tvfp		  22				lower_margin=22
						
			行同步时序：	
				HSPW					thpw		  1<x40   x=20		hsync_len=20
				HBPD					thb-thpw	  46-thpw  26		left_margin=26
				HOZVAL					thd			 	800				xres=800
				HFPD					thfp    		210				right_marin=210

				
				
二、移植lcd屏的方法	
	1--修改时序代码：
		vim arch/arm/mach-s5pv210/mach-smdkv210.c
			214 static struct s3c_fb_pd_win smdkv210_fb_win0 = {
			215     .win_mode = {
			216         .left_margin    = 26,
			217         .right_margin   = 210,
			218         .upper_margin   = 13,
			219         .lower_margin   = 22,
			220         .hsync_len      = 20,
			221         .vsync_len      = 10,
			222         .xres           = 800,
			223         .yres           = 480,
			224     },
			225     .max_bpp    = 32,
			226     .default_bpp        = 16, //和应用有关
			227 };
			

	2--添加背光--再开机的就打开背光
		smdkv210_machine_init(void)
			|
			364     gpio_request(S5PV210_GPD0(0), "GPD0");
			365     gpio_direction_output(S5PV210_GPD0(0), 1);
			366     gpio_free(S5PV210_GPD0(0));

	3-将fbmem.c和s3c-fb.c的驱动编译到内核
		 make menuconfig
			Device Drivers  --->
				Graphics support  ---> 
					 <*> Support for frame buffer devices  --->  //  fbmem.c
						 <*>   Samsung S3C framebuffer support // s3c-fb.c
					
					Console display driver support  ---> 
						 <*> Framebuffer Console support  //将启动logo放入到显存中去显示
					[*] Bootup logo  ---> //将图片编译进内核
						[*]   Standard black and white Linux logo (NEW)
						[*]   Standard 16-color Linux logo (NEW)
						[*]   Standard 224-color Linux logo (NEW)


===============================================================================================						
三、定制一个启动logo：
		
		1.把图片保存到(桌面)
		
		2.用专用软件修改图片 fengjing.jpg
			2.1修改像素  
					图像--->
						宽---800  长----480
					
			2.2修改最大颜色数量
					图像--->模式--->索引
						最大颜色数量---改为224
			2.3另存为(桌面)
					格式转换为.ppm
					
			2.4把.ppm格式的图片文件 拷贝 到						
					\\192.168.7.99\farsight\S5PV210\kernel\linux-3.0.8\drivers\video\logo
					 以---logo_fengjing_clut224.ppm(文件名)--->保存
					 
		3.Kconfig: 提供一个宏：
			/home/farsight/S5PV210/kernel/linux-3.0.8/drivers/video/logo$
				 执行-----vim Kconfig
				
				/** 1.添加代码 **/
					config LOGO_FENGJING_CLUT224
						tristate "Fengjing 224-color Linux logo"
						help
							this is a simple fengjing logo
						default n
		4.Makefile： 编译成对应的文件
			/home/farsight/S5PV210/kernel/linux-3.0.8/drivers/video/logo$/drivers/video/logo$
				执行-----vim Makefile

				y-->.o/m-->.ko/n-->不编译
				/** 2.添加代码 **/
					obj-$(CONFIG_LOGO_FENGJING_CLUT224)     += logo_fengjing_clut224.o

		5.再次执行 make menuconfig
			/home/farsight/S5PV210/kernel/linux-3.0.8$
				执行------make menuconfig
				
				Device Drivers  --->
					Graphics support  ---> 
						[*] Bootup logo  ---> //将图片编译进内核
							[*]   Standard black and white Linux logo (NEW)
							[*]   Standard 16-color Linux logo (NEW)
							[*]   Standard 224-color Linux logo (NEW)
							<*>   Xiong chumo  224-color Linux logo


		6.指定显示哪张
				/home/farsight/S5PV210/kernel/linux-3.0.8drivers/include/linux/$
					执行----vim linux_logo.h

					 extern const struct linux_logo logo_fengjing_clut224;

		6.修改相应文件
				/home/farsight/S5PV210/kernel/linux-3.0.8drivers/drivers/video/logo$
				执行-----vim logo.c
				
				/** 3.添加代码 **/
					#ifdef CONFIG_LOGO_FENGJING_CLUT224
									logo = &logo_fengjing_clut224;
					#endif
					
					
		7. make zImage -j2



				
				
				
				