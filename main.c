#include <linux/module.h>

MODULE_AUTHOR("Luis R. Rodriguez");
MODULE_DESCRIPTION("Kernel compatibility module");
MODULE_LICENSE("GPL");

static int __init compat_init(void)
{
        return 0;
}
module_init(compat_init);

static void __exit compat_exit(void)
{
        return;
}
module_exit(compat_exit);

