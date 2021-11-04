

主要内容----输入子系统
1， 输入子系统的框架和作用
2， 输入子系统中对象和编程方式
3， 输入子系统的代码执行流程
--------------------------------------------
补充：
	1--内核中自带的pdev
	2--读内核代码的技巧，跟读总线中如何匹配，并且如何调用的driver中probe方法

	
	
	

	
输入子系统---框架概念
		1，实现入口函数 xxx_init()和卸载函数 xxx_exit()
		2，申请设备号  register_chrdev_region (与内核相关)
		3，注册字符设备驱动 cdev_alloc  cdev_init  cdev_add (与内核相关)
		4，利用udev/mdev机制创建设备文件(节点) class_create, device_create (与内核相关)
		5，硬件部分初始化
			io资源映射 ioremap,内核提供gpio库函数 (与硬件相关)
			注册中断(与硬件相关)
			初始化等待队列 (与内核相关)
			初始化定时器 (与内核相关)
		6，构建 file_operation结构 (与内核相关)
		7，实现操作硬件方法  xxx_open,xxx_read,xxxx_write...(与硬件相关)
		
		以上步骤：需要进行分层归类，我们只需要实现某个层次就ok


	
输入子系统中设备：
		1，键值类---每个按键只是一个数字
				---keyboard, button (按键类)
		2, 绝对数据类---数据有最大值和最小值
				---ts设备，gsenor
		3, 相对数据类---当前数据时相对上一个数据
				--mouse
		
		
		所有输入设备加入到输入子系统的范畴：
			1， 在应用空间形成统一的编程接口
			2， 在驱动编程的时候会形成统一的编程方法
			
			
			
		利用输入子系统去编写一个按键驱动：
			1，构建一个input device对象
			2， 初始化input device
			3,  注册input device
			4, 硬件初始化，并且获取到数据
			
	
	
			struct input_dev {
				//位表
				//evbit表示设备能够产生哪些数据类型
				unsigned long evbit[BITS_TO_LONGS(EV_CNT)];
				unsigned long keybit[BITS_TO_LONGS(KEY_CNT)]; // keybit[24]==768bit
				unsigned long relbit[BITS_TO_LONGS(REL_CNT)];
				unsigned long absbit[BITS_TO_LONGS(ABS_CNT)];
				
				struct device dev; 				//继承父类---表示记录设备的信息

				struct list_head	h_list;
				struct list_head	node; 		//以链表的形式存在
			}	
		
		
			struct input_event {
				struct timeval time; 	//时间戳
				__u16 type; 			//数据的类型
				__u16 code; 			// 具体是哪个按键
				__s32 value; 			// 按键的值
			};
	



	
	
//跟读代码....................................................................................(重点/超难点)	

	如何通过设备节点知道设备到底是什么：
	peter@ubuntu:/sys/class/input/event0/device$ ls
		capabilities  device  event0  id  modalias  name  phys  power  properties  subsystem  uevent  uniq
	peter@ubuntu:/sys/class/input/event0/device$ cat name
		Power Button
		
		
	

	上报数据的时候实际是：
		void input_report_key(struct input_dev *dev, unsigned int code, int value)
			|
			void input_event(struct input_dev *dev, unsigned int type, unsigned int code, int value)
			
		
	
	内核中的输入子系统的源码目录：
		drivers/input
		
		
			-------------------------------
			input handler层：
				evdev.c  joydev.c mousedev.c
			-------------------------------
			核心层：input.c
			---------------------------
			input device---自己写
					内置的输入设备的驱动：
						touchscreen
						keyboard
						mouse
		
		
			如何确保输入子系统的源码已经编译到内核中去：
				make menuconfig
					Device Drivers  --->
						Input device support  ---> 
							-*- Generic input layer (needed for keyboard, mouse, ...)// input.c
								<*>   Mouse interface // input handler层--mousedev.c
								<*>   Event interface// input handler层--evdev.c
								
								[*]   Touchscreens  ---> //内置的输入设备的驱动
		
		
代码：	
-------------------------------------------------------

input handler---evdev.c


static struct input_handler evdev_handler = {
	.event		= evdev_event,
	.connect	= evdev_connect,
	.disconnect	= evdev_disconnect,
	.fops		= &evdev_fops,
	.minor		= EVDEV_MINOR_BASE,
	.name		= "evdev",
	.id_table	= evdev_ids,

};

//注册了当前的input handler
input_register_handler(&evdev_handler);
	|
	INIT_LIST_HEAD(&handler->h_list);
	//把当前input handler放在全局的数组中
	//evdev_handler-->input_table[2];
	input_table[handler->minor >> 5] = handler;

	//将当前的handler注册到链表中
	list_add_tail(&handler->node, &input_handler_list);

	//遍历input_dev_list
	list_for_each_entry(dev, &input_dev_list, node)
		input_attach_handler(dev, handler);
			|
			//先匹配
			id = input_match_device(handler, dev);
			//匹配成功之后就会调用handler中connect方法
			handler->connect(handler, dev, id);
			

最终都会调用到input handler中connect方法
.connect	= evdev_connect,
		|
		// struct evdev是一个中间产物，记录handler和input_dev关系的对象
		struct evdev *evdev;
		
		// 在全局的evdev_table中找到一个没有占用的位置
		for (minor = 0; minor < EVDEV_MINORS; minor++)
			if (!evdev_table[minor])
				break;
			
		// 分配一个evdev对象
		evdev = kzalloc(sizeof(struct evdev), GFP_KERNEL);
		
		// 初始化--
		INIT_LIST_HEAD(&evdev->client_list);
		spin_lock_init(&evdev->client_lock);
		mutex_init(&evdev->mutex);
		// 初始化等待队列
		init_waitqueue_head(&evdev->wait);
		
		
		// 设置设备名字 --  event0, event1, event2...
		dev_set_name(&evdev->dev, "event%d", minor);
		evdev->exist = true;
		evdev->minor = minor;
		// 创建了设备节点---/dev/event0,event1
		evdev->dev.devt = MKDEV(INPUT_MAJOR, EVDEV_MINOR_BASE + minor);
		evdev->dev.class = &input_class;
		evdev->dev.parent = &dev->dev;
		evdev->dev.release = evdev_free;
		device_initialize(&evdev->dev);
		device_add(&evdev->dev);
		
		
		//记录当前的input handler和input device 到input handle(儿子)
		evdev->handle.dev = input_get_device(dev);
		evdev->handle.name = dev_name(&evdev->dev);
		evdev->handle.handler = handler;
		// 相互关联
		evdev->handle.private = evdev;
		
		// 将当前的handle注册到input handler和input device中个链表中
		//  相互形成关联
		input_register_handle(&evdev->handle);
			|
			list_add_tail_rcu(&handle->d_node, &dev->h_list);
			list_add_tail_rcu(&handle->h_node, &handler->h_list);
		
		//将当前的evdev放入到数组--evdev_table
		evdev_install_chrdev(evdev);
			|
			evdev_table[evdev->minor] = evdev;
	
----------------------------------------------------------
input.c层：
	static LIST_HEAD(input_dev_list);
	static LIST_HEAD(input_handler_list);
	
	
	//  创建类--/sys/class/input
	err = class_register(&input_class);
	// 在 /proc目录中创建文件---/proc/bus/input/devices+handlers
	err = input_proc_init();
	// 注册主设备号--13，注册fops
	err = register_chrdev(INPUT_MAJOR, "input", &input_fops);


-----------------------------------------------------------------
simple_input_key.c 

	input_register_device(key_inputdev);
		|
		list_add_tail(&dev->node, &input_dev_list);

		list_for_each_entry(handler, &input_handler_list, node)
			input_attach_handler(dev, handler);
				|
				//先匹配
				id = input_match_device(handler, dev);
				//匹配成功之后就会调用handler中connect方法
				handler->connect(handler, dev, id);




				
				



创建设备节点：
device_create(struct class * cls,struct device * parent,dev_t devt,void * drvdata,const char * fmt,...)
	|
	dev = device_create_vargs(class, parent, devt, drvdata, fmt, vargs);
		|
		struct device *dev = NULL;
		dev = kzalloc(sizeof(*dev), GFP_KERNEL);
		
		dev->devt = devt;
		dev->class = class;
		dev->parent = parent;
		dev->release = device_create_release;
		dev_set_drvdata(dev, drvdata);
		
		device_register(dev);
			|
			device_initialize(dev);
			return device_add(dev);

		
	对比一下输入子系统中代码：--以下代码其实就是device_create
		evdev->dev.devt = MKDEV(INPUT_MAJOR, EVDEV_MINOR_BASE + minor);
		evdev->dev.class = &input_class;
		evdev->dev.parent = &dev->dev;
		evdev->dev.release = evdev_free;
		device_initialize(&evdev->dev);
		device_add(&evdev->dev);

		
		
---------------------------------------------------------
应用程序如何与地层驱动交互：

	open("/dev/event0");
	read(fd, &event, );
-----------------------------
驱动层中：
	xxx_open()
	
	input.c--
		.open = input_open_file,
			|
			struct input_handler *handler;
			handler = input_table[2];
			// 得到handler中fops
			new_fops = fops_get(handler->fops);
			
			//将handler->fops赋值给file，意味着后面调用read， write，ioctl，都是调用handler 中fops
			// 将文件操作接口转移到 input handler 层
			file->f_op = new_fops;

			err = new_fops->open(inode, file); //实际是evdev_open
	---------------------------------------------------
	evdev.c层：	
		evdev_fops = {
			.open		= evdev_open,
					|
					struct evdev *evdev;
					
					//根据次设备号-64得到0, 1,2,实际就是数组的下标
					int i = iminor(inode) - EVDEV_MINOR_BASE;
					// 得到evdev--通过eventX找到对应的evdev
					evdev = evdev_table[i];
					
					//分配缓冲队列
					bufsize = evdev_compute_buffer_size(evdev->handle.dev);

					client = kzalloc(sizeof(struct evdev_client) +
								bufsize * sizeof(struct input_event),
							 GFP_KERNEL);
					// 相互记录client和evdev	 
					client->bufsize = bufsize;
					spin_lock_init(&client->buffer_lock);
					client->evdev = evdev;
					evdev_attach_client(evdev, client);
						|
						list_add_tail_rcu(&client->node, &evdev->client_list);
					
					// 尝试调用input device中open方法--实际是没有的
					evdev_open_device(evdev);
					
					//将client记录到file中私有数据
					file->private_data = client;
					
		}


		evdev_read()
			|
			//如果没有数据，就需要阻塞
			retval = wait_event_interruptible(evdev->wait,
				client->packet_head != client->tail || !evdev->exist);
			
			while (retval + input_event_size() <= count &&
					// 从client对应缓冲去中获取struct input_event数据包
					evdev_fetch_next_event(client, &event)) {
							|
							have_event = client->head != client->tail;
							if (have_event) {
								*event = client->buffer[client->tail++];
								client->tail &= client->bufsize - 1;
							}
				//拷贝给用户
				if (input_event_to_user(buffer + retval, &event))
						|
					copy_to_user(buffer, event, sizeof(struct input_event)
	

		是谁唤醒的，并且谁将数据放在client的缓冲区？
		
		定位到 input device中，如何上报数据的？
		input_event(key_inputdev, EV_KEY, p->code, !!0);
			|
			input_handle_event(dev, type, code, value);
				|
				switch (type) {
					case EV_KEY:
					{
						disposition = INPUT_PASS_TO_HANDLERS;
						
					}
			
				if (disposition & INPUT_PASS_TO_HANDLERS)
					input_pass_event(dev, type, code, value);
						|
						struct input_handler *handler;
						struct input_handle *handle;
						list_for_each_entry_rcu(handle, &dev->h_list, d_node)
							//通过儿子找到老子
							handler = handle->handler;
							//调用了handler中event(handle, type, code, value)
							handler->event(handle, type, code, value);

				
				跑到evdev.c
					.event		= evdev_event,
						|
						struct evdev *evdev = handle->private;
						struct evdev_client *client;
						struct input_event event;
						
						//封包--struct input_event
						do_gettimeofday(&event.time);
						event.type = type;
						event.code = code;
						event.value = value;

						rcu_read_lock();
						//得到缓冲区
						client = rcu_dereference(evdev->grab);
						if (client)
							evdev_pass_event(client, &event);
								|
								//将底层得到struct input_event放入到缓冲队列
								client->buffer[client->head++] = *event;
								
									if (event->type == EV_SYN && event->code == SYN_REPORT) {
										client->packet_head = client->head;
										//发送异步信号
										kill_fasync(&client->fasync, SIGIO, POLL_IN);
									}
						// 唤醒等待队列---input_sync(key_inputdev);					
						if (type == EV_SYN && code == SYN_REPORT)
							wake_up_interruptible(&evdev->wait);
	
	
