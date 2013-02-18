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

void netdev_set_default_ethtool_ops(struct net_device *dev,
				    const struct ethtool_ops *ops)
{
	if (!dev->ethtool_ops)
		dev->ethtool_ops = ops;
}
EXPORT_SYMBOL_GPL(netdev_set_default_ethtool_ops);
