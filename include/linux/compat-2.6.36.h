#ifndef LINUX_26_36_COMPAT_H
#define LINUX_26_36_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))

#define kparam_block_sysfs_write(a)
#define kparam_unblock_sysfs_write(a)

struct va_format {
	const char *fmt;
	va_list *va;
};

#define device_rename(dev, new_name) device_rename(dev, (char *)new_name)

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) */

#endif /* LINUX_26_36_COMPAT_H */
