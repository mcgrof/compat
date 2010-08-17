/*
 * Copyright 2010    Hauke Mehrtens <hauke@hauke-m.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.37.
 */

#include <linux/compat.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)

#include <linux/netdevice.h>
#include <net/sock.h>
#include <linux/nsproxy.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
static const void *net_current_ns(void)
{
	return current->nsproxy->net_ns;
}

static const void *net_initial_ns(void)
{
	return &init_net;
}

static const void *net_netlink_ns(struct sock *sk)
{
	return sock_net(sk);
}

struct kobj_ns_type_operations net_ns_type_operations = {
	.type = KOBJ_NS_TYPE_NET,
	.current_ns = net_current_ns,
	.netlink_ns = net_netlink_ns,
	.initial_ns = net_initial_ns,
};
EXPORT_SYMBOL_GPL(net_ns_type_operations);

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)*/ 

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37) */
