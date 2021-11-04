
#include <linux/init.h>
#include <linux/module.h>


int my_add(int a, int b)
{
    return a + b;
}
/************************************************
**  函 数 名：EXPORT_SYMBOL 
**  函数功能：实现不同模块的相互调用
**  参数说明：被调函数的函数名
**  返 回 值：？？？？？？？？？
************************************************/
EXPORT_SYMBOL(my_add);


int my_sub(int a, int b)
{
    return a - b;
}
EXPORT_SYMBOL(my_sub);

MODULE_LICENSE("GPL");

