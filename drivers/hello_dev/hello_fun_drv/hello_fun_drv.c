

#include <linux/init.h>
#include <linux/module.h>
#include "fun.h"

static int __init hello_init(void)
{
    printk("a + b = %d\n", my_add(5, 3));
    printk("a - b = %d\n", my_sub(5, 3));
    printk("========^_^=====%s====\n", __func__);

    return 0;
}


static void __exit hello_exit(void)
{
    printk("========^_^=====%s====\n", __func__);
}

module_init(hello_init);
module_exit(hello_exit);



MODULE_LICENSE("GPL");

