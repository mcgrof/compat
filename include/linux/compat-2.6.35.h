#ifndef LINUX_26_35_COMPAT_H
#define LINUX_26_35_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))

#define IW_HANDLER(id, func)			\
	[IW_IOCTL_IDX(id)] = func

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)) */

#endif /* LINUX_26_35_COMPAT_H */
