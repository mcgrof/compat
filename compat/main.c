#include <linux/module.h>

MODULE_AUTHOR("Luis R. Rodriguez");
MODULE_DESCRIPTION("Kernel compatibility module");
MODULE_LICENSE("GPL");

#ifndef COMPAT_VERSION
#error "You need a COMPAT_VERSION"
#endif

static char *compat_version = COMPAT_VERSION;

module_param(compat_version, charp, 0400);
MODULE_PARM_DESC(compat_version, "Version of the kernel compat backport");

static int __init compat_init(void)
{
	/* pm-qos for kernels <= 2.6.24, this is a no-op on newer kernels */
	compat_pm_qos_power_init();

	printk(KERN_INFO "Generic kernel compatibility enabled based on "
	       COMPAT_VERSION
	       "\n");

        return 0;
}
module_init(compat_init);

static void __exit compat_exit(void)
{
	compat_pm_qos_power_deinit();

        return;
}
module_exit(compat_exit);

