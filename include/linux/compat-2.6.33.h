#ifndef LINUX_26_33_COMPAT_H
#define LINUX_26_33_COMPAT_H

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/compat_autoconf.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))

#define IFF_DONT_BRIDGE 0x800		/* disallow bridging this ether dev */
/* source: include/linux/if.h */

#define kparam_block_sysfs_write(a)
#define kparam_unblock_sysfs_write(a)

/* this will never happen on older kernels */
#define NETDEV_POST_INIT 0xffff

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)) */

#endif /* LINUX_26_33_COMPAT_H */
