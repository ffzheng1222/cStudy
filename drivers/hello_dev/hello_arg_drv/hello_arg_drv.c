

#include <linux/init.h>
#include <linux/module.h>


int a = 125;
char *str = "hello,world!!!"; 


static int __init hello_init(void)
{
    printk("a = %d\n", a);
    printk("str = %s\n", str);
    printk("========^_^=====%s====\n", __func__);

    return 0;
}


/**********************************************************
**  函 数 名：module_param
**  函数功能：实现模块传参的方法
**  参数说明：a ：变量
**	      int ：变量的类型
**	      0644 ：权限(因为每一个变量在内核中都
**		        是以文件的形式进行传参的)
**  返 回 值：？？？？？？？？？？？
**  注 意 点：此函数一般用在变量使用完之后使用
***********************************************************/
module_param(a, int, 0644);
module_param(str, charp, 0644);


static void __exit hello_exit(void)
{
    printk("========^_^=====%s====\n", __func__);
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_LICENSE("GPL");

