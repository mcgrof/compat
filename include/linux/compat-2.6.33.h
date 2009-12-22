#ifndef LINUX_26_33_COMPAT_H
#define LINUX_26_33_COMPAT_H

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/compat_autoconf.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))

#include <linux/skbuff.h>
#include <pcmcia/cs_types.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>

#define IFF_DONT_BRIDGE 0x800		/* disallow bridging this ether dev */
/* source: include/linux/if.h */

#define kparam_block_sysfs_write(a)
#define kparam_unblock_sysfs_write(a)

/* this will never happen on older kernels */
#define NETDEV_POST_INIT 0xffff

static inline struct sk_buff *netdev_alloc_skb_ip_align(struct net_device *dev,
                unsigned int length)
{
	struct sk_buff *skb = netdev_alloc_skb(dev, length + NET_IP_ALIGN);

	if (NET_IP_ALIGN && skb)
		skb_reserve(skb, NET_IP_ALIGN);
	return skb;
}

#define pcmcia_request_window(a, b, c) pcmcia_request_window(&a, b, c)

#define pcmcia_map_mem_page(a, b, c) pcmcia_map_mem_page(b, c)

/* loop over CIS entries */
int pcmcia_loop_tuple(struct pcmcia_device *p_dev, cisdata_t code,
		      int (*loop_tuple) (struct pcmcia_device *p_dev,
					 tuple_t *tuple,
					 void *priv_data),
		      void *priv_data);

/* loop over CIS entries */
int pccard_loop_tuple(struct pcmcia_socket *s, unsigned int function,
		      cisdata_t code, cisparse_t *parse, void *priv_data,
		      int (*loop_tuple) (tuple_t *tuple,
					 cisparse_t *parse,
					 void *priv_data));

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)) */

#endif /* LINUX_26_33_COMPAT_H */
