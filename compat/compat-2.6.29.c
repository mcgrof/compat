/*
 * Copyright 2007-2010	Luis R. Rodriguez <mcgrof@winlab.rutgers.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.29.
 */

#include <linux/compat.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29))

#include <linux/usb.h>
#include <linux/etherdevice.h>

/*
 * Expand this as drivers require more ops, for now this
 * only sets the ones we need.
 */
void netdev_attach_ops(struct net_device *dev,
		       const struct net_device_ops *ops)
{
#define SET_NETDEVOP(_op) (_op ? _op : NULL)
	dev->open = SET_NETDEVOP(ops->ndo_open);
	dev->stop = SET_NETDEVOP(ops->ndo_stop);
	dev->hard_start_xmit = SET_NETDEVOP(ops->ndo_start_xmit);
	dev->set_multicast_list = SET_NETDEVOP(ops->ndo_set_multicast_list);
	dev->change_mtu = SET_NETDEVOP(ops->ndo_change_mtu);
	dev->set_mac_address = SET_NETDEVOP(ops->ndo_set_mac_address);
	dev->tx_timeout = SET_NETDEVOP(ops->ndo_tx_timeout);
	dev->get_stats = SET_NETDEVOP(ops->ndo_get_stats);
#undef SET_NETDEVOP
}
EXPORT_SYMBOL(netdev_attach_ops);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23))
#if defined(CONFIG_USB) || defined(CONFIG_USB_MODULE)
/**
 * usb_unpoison_anchored_urbs - let an anchor be used successfully again
 * @anchor: anchor the requests are bound to
 *
 * Reverses the effect of usb_poison_anchored_urbs
 * the anchor can be used normally after it returns
 */
void usb_unpoison_anchored_urbs(struct usb_anchor *anchor)
{
	unsigned long flags;
	struct urb *lazarus;

	spin_lock_irqsave(&anchor->lock, flags);
	list_for_each_entry(lazarus, &anchor->urb_list, anchor_list) {
		usb_unpoison_urb(lazarus);
	}
	//anchor->poisoned = 0; /* XXX: cannot backport */
	spin_unlock_irqrestore(&anchor->lock, flags);
}
EXPORT_SYMBOL_GPL(usb_unpoison_anchored_urbs);
#endif /* CONFIG_USB */
#endif

/**
 * eth_mac_addr - set new Ethernet hardware address
 * @dev: network device
 * @p: socket address
 * Change hardware address of device.
 *
 * This doesn't change hardware matching, so needs to be overridden
 * for most real devices.
 */
int eth_mac_addr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;

	if (netif_running(dev))
		return -EBUSY;
	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;
	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	return 0;
}
EXPORT_SYMBOL(eth_mac_addr);
/* Source: net/ethernet/eth.c */


#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29) */

