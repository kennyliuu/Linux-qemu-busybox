#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void)
{
    pr_info("👋 Hello World from module\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_info("👋 Goodbye World from module\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kenny");
MODULE_DESCRIPTION("Hello World kernel module using pr_info()");
