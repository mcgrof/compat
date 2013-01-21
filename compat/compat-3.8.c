/*
 * Copyright 2018  Luis R. Rodriguez <mcgrof@do-not-panic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 3.8.
 */

#include <linux/netdevice.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,5))

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))
void netdev_set_default_ethtool_ops(struct net_device *dev,
				    const struct ethtool_ops *ops)
{
	if (!dev->ethtool_ops)
		dev->ethtool_ops = ops;
}
#else /* kernel is between 3.7.0 and 3.7.4 */;
void netdev_set_default_ethtool_ops(struct net_device *dev,
				    const struct ethtool_ops *ops)
{
	if (dev->ethtool_ops == &default_ethtool_ops)
		dev->ethtool_ops = ops;
}
#endif

EXPORT_SYMBOL_GPL(netdev_set_default_ethtool_ops);
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,5) */
