#ifndef LINUX_26_33_COMPAT_H
#define LINUX_26_33_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33))

#include <linux/skbuff.h>
#if defined(CONFIG_PCCARD) || defined(CONFIG_PCCARD_MODULE)
#include <pcmcia/cs_types.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>
#endif
#include <linux/kfifo.h>
#include <linux/firmware.h>

#define release_firmware compat_release_firmware
#define request_firmware compat_request_firmware
#define request_firmware_nowait compat_request_firmware_nowait

#if defined(CONFIG_FW_LOADER) || defined(CONFIG_FW_LOADER_MODULE)
int compat_request_firmware(const struct firmware **fw, const char *name,
		     struct device *device);
int compat_request_firmware_nowait(
	struct module *module, int uevent,
	const char *name, struct device *device, gfp_t gfp, void *context,
	void (*cont)(const struct firmware *fw, void *context));

void compat_release_firmware(const struct firmware *fw);
#else
static inline int compat_request_firmware(const struct firmware **fw,
				   const char *name,
				   struct device *device)
{
	return -EINVAL;
}
static inline int request_firmware_nowait(
	struct module *module, int uevent,
	const char *name, struct device *device, gfp_t gfp, void *context,
	void (*cont)(const struct firmware *fw, void *context))
{
	return -EINVAL;
}

static inline void compat_release_firmware(const struct firmware *fw)
{
}
#endif

#define KEY_RFKILL		247	/* Key that controls all radios */

#define IFF_DONT_BRIDGE 0x800		/* disallow bridging this ether dev */
/* source: include/linux/if.h */

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

#if defined(CONFIG_PCCARD) || defined(CONFIG_PCCARD_MODULE)

#if defined(CONFIG_PCMCIA) || defined(CONFIG_PCMCIA_MODULE)

#define pcmcia_request_window(a, b, c) pcmcia_request_window(&a, b, c)

#define pcmcia_map_mem_page(a, b, c) pcmcia_map_mem_page(b, c)

/* loop over CIS entries */
int pcmcia_loop_tuple(struct pcmcia_device *p_dev, cisdata_t code,
		      int (*loop_tuple) (struct pcmcia_device *p_dev,
					 tuple_t *tuple,
					 void *priv_data),
		      void *priv_data);

#endif /* CONFIG_PCMCIA */

/* loop over CIS entries */
int pccard_loop_tuple(struct pcmcia_socket *s, unsigned int function,
		      cisdata_t code, cisparse_t *parse, void *priv_data,
		      int (*loop_tuple) (tuple_t *tuple,
					 cisparse_t *parse,
					 void *priv_data));

#endif /* CONFIG_PCCARD */

/* Backport for kfifo
 * kfifo_alloc and kfifo_free must be backported manually 
 */
#define kfifo_in(a, b, c) __kfifo_put(*a, (unsigned char *)b, c)
#define kfifo_out(a, b, c) __kfifo_get(*a, (unsigned char *)b, c)
#define kfifo_len(a) __kfifo_len(*a)

/**
 * kfifo_is_empty - returns true if the fifo is empty
 * @fifo: the fifo to be used.
 */
static inline __must_check int kfifo_is_empty(struct kfifo **fifo)
{
	return (*fifo)->in == (*fifo)->out;
}

/**
 * kfifo_size - returns the size of the fifo in bytes
 * @fifo: the fifo to be used.
 */
static inline __must_check unsigned int kfifo_size(struct kfifo *fifo)
{
	return fifo->size;
}

/**
 * kfifo_is_full - returns true if the fifo is full
 * @fifo: the fifo to be used.
 */
static inline __must_check int kfifo_is_full(struct kfifo **fifo)
{
	return kfifo_len(fifo) == kfifo_size(*fifo);
}

static inline void compat_kfifo_free(struct kfifo **fifo) {
	if (*fifo)
		kfifo_free(*fifo);
}
#define kfifo_free compat_kfifo_free

/**
 * list_for_each_entry_continue_rcu - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_rcu(pos, head, member) 		\
	for (pos = list_entry_rcu(pos->member.next, typeof(*pos), member); \
	     prefetch(pos->member.next), &pos->member != (head);	\
	     pos = list_entry_rcu(pos->member.next, typeof(*pos), member))

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)) */

#endif /* LINUX_26_33_COMPAT_H */
