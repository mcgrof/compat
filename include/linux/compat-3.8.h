#ifndef LINUX_3_8_COMPAT_H
#define LINUX_3_8_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0))

#include <linux/netdevice.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,8))
extern void netdev_set_default_ethtool_ops(struct net_device *dev,
					   const struct ethtool_ops *ops);
#endif

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0)) */

#endif /* LINUX_3_8_COMPAT_H */
