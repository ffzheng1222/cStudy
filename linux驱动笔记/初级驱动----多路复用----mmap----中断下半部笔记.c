
主要内容
1, 	多路复用的实现
2, 	mmap的实现
3,	中断下半部的实现方式
		a, tasklet的实现
		b, 工作队列的实现
------------------------------------------------------------



应用调用:
	open()
------------------------------------------------------------
vfs:
	SYSCALL_DEFINE3(open, const char __user *, filename, int, flags, int, mode)
	sys_open()--fs/open.c
	{
		do_sys_open(AT_FDCWD, filename, flags, mode);
			|
			
			//  1, 创建struct file 记录open中各个参数信息, 返回一个fd, 将fd和struct file关联]
			fd = get_unused_fd_flags(flags);
			struct file *f = do_filp_open(dfd, tmp, &op, lookup);
			fd_install(fd, f);
			//  2,查找cdev的代码--在do_filp_open
					|chrdev_open查找cdev
					const struct file_operations def_chr_fops = {
						.open = chrdev_open,
								|
								struct cdev *new = NULL;
								//根据设备号找到cdev中kobj
								kobj = kobj_lookup(cdev_map, inode->i_rdev, &idx);
								//通过kobj找到cdev
								new = container_of(kobj, struct cdev, kobj);
								inode->i_cdev = p = new;
								//将cdev中fop给了file的f_op
								filp->f_op = fops_get(p->ops);
								//调用了cdev中fop的open方法
								filp->f_op->open(inode,filp);
								
						.llseek = noop_llseek,
					};
	}
	
------------------------------------------------------------
	驱动:
		const struct file_operations key_fops = {
			.open = key_drv_open,
			.write = key_drv_write,
			.read = key_drv_read,
			.release = key_drv_close,
		};
		
	
	

应用层:
		write()   ioctl()
------------------------------------------------------------
vfs :
	SYSCALL_DEFINE3(write
	sys_write()--fs/read_write.c
	{
		file = fget_light(fd, &fput_needed);
		vfs_write(file, buf, count, &pos);
			|
			if (file->f_op->write)
				ret = file->f_op->write(file, buf, count, pos);
	
	}
	
	SYSCALL_DEFINE3(ioctl
	sys_ioctl()--fs/ioctl.c
	{
		filp = fget_light(fd, &fput_needed);
		do_vfs_ioctl(filp, fd, cmd, arg);
			|
			 vfs_ioctl(filp, cmd, arg);
				|
				 filp->f_op->unlocked_ioctl(filp, cmd, arg);
	}
	------------------------------------------------------------
		const struct file_operations key_fops = {
			.unlocked_ioctl = led_drv_ioctl,
		};
	
	
	
	
	
	
	
1, 多路复用的实现

	struct pollfd {
		int fd; 			//被监控的fd
		short events;		//希望监控的事件:读,写,出错
		short revents;		//用于查询,当前fd是否发生了读,写,出错
	};	
	
	//监控两个fd---标准输入,和当前key按键
	struct pollfd pfds[2];

	pfds[0].fd = 0;
	pfds[0].events = POLLIN ;

	pfds[1].fd = fd;
	pfds[1].events = POLLIN;



	while(1)
	{	
		ret = poll(pfds, 2,  -1);
		
	}
	
	--------------------------------------
	驱动中实现poll接口
		unsigned int key_drv_poll(struct file *filp, struct poll_table_struct *pts)
		{
			unsigned int mask = 0;
			
			//  1, 将当前的等待队列头注册到vfs中
			/ ************************************************************************
			**	函 数 名：poll_wait
			**	函数功能：将当前的等待队列头注册到vfs中
			**  参数说明：参数1：struct file对象
			**			  参数2：等待队列头
			**			  参数3：与等待队列关联的表格
			**	返 回 值：无
			**	头 文 件：#include <linux/poll.h>
			*************************************************************************/
			static inline void poll_wait(struct file * filp, wait_queue_head_t * wait_address, poll_table *p)


			// 2, 如果有数据返回一个pollin,没有数据返回一个0
			if(key_dev->have_data)
			{
				mask |= POLLIN;
			}
			return mask;
		}
	
	
	
2, mmap的实现
	1,是文件io中一种
	2,进程空间和驱动数据交互的比较高效的方式
	3, 将内核空间的物理内存映射到用户空间,直接可以操作地址
	
	
	
	#include <sys/mman.h>
			/ ************************************************************************
			**	函 数 名：mmap
			**	函数功能：将内核空间的物理内存映射到用户空间,直接可以操作地址
			**  参数说明：参数1：指定映射到用户空间的地址,一般都填NULL,由系统自动分配
			**			  参数2：映射的长度
			**			  参数3：对内存的访问权限PROT_EXEC,PROT_READ,PROT_WRITE
			**			  参数4：是否允许其他进程映射---MAP_PRIVATE,MAP_SHARED(允许共享)
			**			  参数5：打开的文件
			**			  参数6：从内存的多少偏移量开始映射
			**	返 回 值：成功：返回映射到用户空间的地址
			**			  失败：返回MAP_FAILED
			**	头 文 件：#include <sys/mman.h>
			*************************************************************************/
        void *mmap(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
     
			/ ************************************************************************
			**	函 数 名：munmap
			**	函数功能：撤销mmap函数映射的地址
			**  参数说明：参数1：mmap函数成功映射到用户空间的地址
			**			  参数2：mmap函数映射地址的长度
			**	返 回 值：成功：返回 	0
			**			  失败：返回 	-1
			**	头 文 件：#include <linux/poll.h>
			*************************************************************************/
	    int munmap(void *addr, size_t length);
	 
	 
	 ---------------------------------------------------------------
	 驱动实现: 实现fops中mmap接口
		int (*mmap) (struct file *, struct vm_area_struct *);

		int key_drv_mmap(struct file *filp, struct vm_area_struct *vma)
		{

			unsigned long addr;

			// 只要调用io_remap_pfn_range就能将驱动中内存映射到用户空间
			//vma用于描述用户空间映射的需求, vma是vfs层传递过来

			addr = virt_to_phys(key_dev->virt_mem);

			vma->vm_flags |= VM_IO;
			vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
			
				/ ************************************************************************
				**	带 参 宏：io_remap_pfn_range
				**	宏 功 能：将内核空间的物理内存映射到用户空间,直接可以操作地址
				**  参数说明：参数1：表示需求
				**			  参数2：映射到用户空间的起始位置
				**			  参数3：被映射的物理地址的页地址
				**			  参数4：映射的大小
				**			  参数5：映射的权限
				**	返 回 值：成功：映射成功
				**			  失败：返回 -EAGAIN 错误码
				**	头 文 件：#include <linux/mm.h>
				*************************************************************************/
			if (io_remap_pfn_range(vma, vma->vm_start, addr >> PAGE_SHIFT,
							PAGE_SIZE, vma->vm_page_prot)) {
				printk(KERN_ERR "%s: io_remap_pfn_range failed\n",
					__func__);
				return -EAGAIN;
			}

			return 0;

		}

	
	
3中断下半部的编程:
		//中断下半部实现对象
		struct tasklet_struct		
		{
			struct tasklet_struct *next;
			unsigned long state;
			atomic_t count;
			void (*func)(unsigned long);
			unsigned long data;
		};
	
		1, 初始化tasklet
				/ ************************************************************************
				**	函 数 名：tasklet_init
				**	函数功能：初始化中断下半部
				**  参数说明：参数1：中断下半部实现对象
				**			  参数2：中断下半部处理函数
				**			  参数3：为中断下半部处理函数传的参数
				**	返 回 值：无
				**	头 文 件：#include <linux/interrupt.h>
				*************************************************************************/
			void tasklet_init(struct tasklet_struct *t, void (*func)(unsigned long), unsigned long data)
		
		2, 在中断的上半部将tasklet加入到内核线程中
				/ ************************************************************************
				**	函 数 名：tasklet_schedule
				**	函数功能：(在中断上半部中)启用中断下半部
				**  参数说明：参数1：中断下半部实现对象
				**	返 回 值：无
				**	头 文 件：#include <linux/interrupt.h>
				*************************************************************************/
			static inline void tasklet_schedule(struct tasklet_struct *t)
			
		3, 销毁tasklet
				/ ************************************************************************
				**	函 数 名：tasklet_kill
				**	函数功能：销毁中断下半部
				**  参数说明：参数1：中断下半部实现对象
				**	返 回 值：无
				**	头 文 件：#include <linux/interrupt.h>
				*************************************************************************/
			void tasklet_kill(struct tasklet_struct *t)
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	



