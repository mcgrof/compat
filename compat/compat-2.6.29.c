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
 * If you don't see your net_device_ops implemented on
 * netdev_attach_ops() then you are shit out of luck and
 * you must do the nasty ifdef magic, unless you figure
 * out a way to squeze your hacks into this routine :)
 */
void netdev_attach_ops(struct net_device *dev,
		       const struct net_device_ops *ops)
{
	dev->open = ops->ndo_open;
	dev->stop = ops->ndo_stop;
	dev->hard_start_xmit = ops->ndo_start_xmit;
	dev->change_rx_flags = ops->ndo_change_rx_flags;
	dev->set_multicast_list = ops->ndo_set_multicast_list;
	dev->validate_addr = ops->ndo_validate_addr;
	dev->do_ioctl = ops->ndo_do_ioctl;
	dev->set_config = ops->ndo_set_config;
	dev->change_mtu = ops->ndo_change_mtu;
	dev->set_mac_address = ops->ndo_set_mac_address;
	dev->tx_timeout = ops->ndo_tx_timeout;
	dev->get_stats = ops->ndo_get_stats;
	dev->vlan_rx_register = ops->ndo_vlan_rx_register;
	dev->vlan_rx_add_vid = ops->ndo_vlan_rx_add_vid;
	dev->vlan_rx_kill_vid = ops->ndo_vlan_rx_kill_vid;
#ifdef CONFIG_NET_POLL_CONTROLLER
	dev->poll_controller = ops->ndo_poll_controller;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
	dev->select_queue = ops->ndo_select_queue;
#endif
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

/**
 * eth_change_mtu - set new MTU size
 * @dev: network device
 * @new_mtu: new Maximum Transfer Unit
 *
 * Allow changing MTU size. Needs to be overridden for devices
 * supporting jumbo frames.
 */
int eth_change_mtu(struct net_device *dev, int new_mtu)
{
	if (new_mtu < 68 || new_mtu > ETH_DATA_LEN)
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}
EXPORT_SYMBOL(eth_change_mtu);

int eth_validate_addr(struct net_device *dev)
{
	if (!is_valid_ether_addr(dev->dev_addr))
		return -EADDRNOTAVAIL;

	return 0;
}
EXPORT_SYMBOL(eth_validate_addr);
/* Source: net/ethernet/eth.c */


#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,29) */

