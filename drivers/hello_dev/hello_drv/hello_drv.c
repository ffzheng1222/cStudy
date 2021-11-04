﻿

/**********第1步. 头文件 *************/
#include <linux/init.h>
#include <linux/module.h>


/**********第2步.模块加载函数*********/
/**************************************************************
**  函 数 名：int __init hello_init
**  函数功能：模块加载函数(向系统申请资源)
**  参数说明：无
**  返 回 值：0
***************************************************************/
static int __init hello_init(void)
{
    printk("========^_^=====%s====\n", __func__);

    return 0;
}

/**********第3步.模块卸载函数*********/
/**************************************************************
**  函 数 名：void __exit hello_exit
**  函数功能：模块卸载函数(释放系统资源)
**  参数说明：无
**  返 回 值：无
***********************************************************/
static void __exit hello_exit(void)
{
    printk("========^_^=====%s====\n", __func__);
}




/**********第4步.模块加载/卸载函数的声明*********/
/**************************************************************
**  函 数 名：module_init
**  函数功能：模块加载函数的声明
**  参数说明：模块加载函数名
**  返 回 值：无？？？？？？？？
***********************************************************/
module_init(hello_init);
/**************************************************************
**  函 数 名：module_exit
**  函数功能：模块卸载函数的声明
**  参数说明：模块卸载函数名
**  返 回 值：无？？？？？？？？
***********************************************************/
module_exit(hello_exit);




/**********第5步.模块注册*********/
/**************************************************************
**  函 数 名：MODULE_LICENSE
**  函数功能: 模块的注册
**  参数说明：注册标准 “GPL”
**  返 回 值：无？？？？？？？？
***********************************************************/
MODULE_LICENSE("GPL");
