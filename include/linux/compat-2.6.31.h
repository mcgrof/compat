#ifndef LINUX_26_31_COMPAT_H
#define LINUX_26_31_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))

#include <linux/skbuff.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <net/dst.h>
#include <net/genetlink.h>
#include <linux/ethtool.h>
#include <net/sock.h>

#define SUPPORTED_Backplane            (1 << 16)
#define SUPPORTED_1000baseKX_Full      (1 << 17)
#define SUPPORTED_10000baseKX4_Full    (1 << 18)
#define SUPPORTED_10000baseKR_Full     (1 << 19)
#define SUPPORTED_10000baseR_FEC       (1 << 20)

#define ADVERTISED_Backplane           (1 << 16)
#define ADVERTISED_1000baseKX_Full     (1 << 17)
#define ADVERTISED_10000baseKX4_Full   (1 << 18)
#define ADVERTISED_10000baseKR_Full    (1 << 19)
#define ADVERTISED_10000baseR_FEC      (1 << 20)

#define rfkill_get_led_trigger_name LINUX_BACKPORT(rfkill_get_led_trigger_name)
#define rfkill_set_led_trigger_name LINUX_BACKPORT(rfkill_set_led_trigger_name)
#define rfkill_set_hw_state LINUX_BACKPORT(rfkill_set_hw_state)
#define rfkill_set_sw_state LINUX_BACKPORT(rfkill_set_sw_state)
#define rfkill_init_sw_state LINUX_BACKPORT(rfkill_init_sw_state)
#define rfkill_set_states LINUX_BACKPORT(rfkill_set_states)
#define rfkill_pause_polling LINUX_BACKPORT(rfkill_pause_polling)
#define rfkill_resume_polling LINUX_BACKPORT(rfkill_resume_polling)
#define rfkill_blocked LINUX_BACKPORT(rfkill_blocked)
#define rfkill_alloc LINUX_BACKPORT(rfkill_alloc)
#define rfkill_register LINUX_BACKPORT(rfkill_register)
#define rfkill_unregister LINUX_BACKPORT(rfkill_unregister)
#define rfkill_destroy LINUX_BACKPORT(rfkill_destroy)

#ifndef ERFKILL
#if !defined(CONFIG_ALPHA) && !defined(CONFIG_MIPS) && !defined(CONFIG_PARISC) && !defined(CONFIG_SPARC)
#define ERFKILL		132	/* Operation not possible due to RF-kill */
#endif
#ifdef CONFIG_ALPHA
#define ERFKILL		138	/* Operation not possible due to RF-kill */
#endif
#ifdef CONFIG_MIPS
#define ERFKILL		167	/* Operation not possible due to RF-kill */
#endif
#ifdef CONFIG_PARISC
#define ERFKILL		256	/* Operation not possible due to RF-kill */
#endif
#ifdef CONFIG_SPARC
#define ERFKILL		134	/* Operation not possible due to RF-kill */
#endif
#endif

#define mdio45_probe LINUX_BACKPORT(mdio45_probe)
#define mdio_set_flag LINUX_BACKPORT(mdio_set_flag)
#define mdio45_links_ok LINUX_BACKPORT(mdio45_links_ok)
#define mdio45_nway_restart LINUX_BACKPORT(mdio45_nway_restart)

#define mdio45_ethtool_gset_npage LINUX_BACKPORT(mdio45_ethtool_gset_npage)
#define mdio45_ethtool_spauseparam_an LINUX_BACKPORT(mdio45_ethtool_spauseparam_an)
#define mdio_mii_ioctl LINUX_BACKPORT(mdio_mii_ioctl)

#ifndef NETDEV_PRE_UP
#define NETDEV_PRE_UP		0x000D
#endif

#ifndef SDIO_DEVICE_ID_MARVELL_8688WLAN
#define SDIO_DEVICE_ID_MARVELL_8688WLAN		0x9104
#endif

struct compat_threaded_irq {
	unsigned int irq;
	irq_handler_t handler;
	irq_handler_t thread_fn;
	void *dev_id;
	char wq_name[64];
	struct workqueue_struct *wq;
	struct work_struct work;
};

/*
 * kmemleak was introduced on 2.6.31, since older kernels do not have
 * we simply ignore its tuning.
 */
static inline void kmemleak_ignore(const void *ptr)
{
	return;
}

static inline void kmemleak_not_leak(const void *ptr)
{
	return;
}

static inline void kmemleak_no_scan(const void *ptr)
{
	return;
}

/*
 * Added via adf30907d63893e4208dfe3f5c88ae12bc2f25d5
 *
 * There is no _sk_dst on older kernels, so just set the
 * old dst to NULL and release it directly.
 */
static inline void skb_dst_drop(struct sk_buff *skb)
{
	dst_release(skb->dst);
	skb->dst = NULL;
}

static inline struct dst_entry *skb_dst(const struct sk_buff *skb)
{
	return (struct dst_entry *)skb->dst;
}

static inline void skb_dst_set(struct sk_buff *skb, struct dst_entry *dst)
{
	skb->dst = dst;
}

static inline struct rtable *skb_rtable(const struct sk_buff *skb)
{
	return (struct rtable *)skb_dst(skb);
}

/* Backport threaded IRQ support */

static inline
void compat_irq_work(struct work_struct *work)
{
	struct compat_threaded_irq *comp = container_of(work, struct compat_threaded_irq, work);
	comp->thread_fn(comp->irq, comp->dev_id);
}

static inline
irqreturn_t compat_irq_dispatcher(int irq, void *dev_id)
{
	struct compat_threaded_irq *comp = dev_id;
	irqreturn_t res;

	res = comp->handler(irq, comp->dev_id);
	if (res == IRQ_WAKE_THREAD) {
		queue_work(comp->wq, &comp->work);
		res = IRQ_HANDLED;
	}

	return res;
}

static inline
int compat_request_threaded_irq(struct compat_threaded_irq *comp,
				unsigned int irq,
				irq_handler_t handler,
				irq_handler_t thread_fn,
				unsigned long flags,
				const char *name,
				void *dev_id)
{
	comp->irq = irq;
	comp->handler = handler;
	comp->thread_fn = thread_fn;
	comp->dev_id = dev_id;
	INIT_WORK(&comp->work, compat_irq_work);

	if (!comp->wq) {
		snprintf(comp->wq_name, sizeof(comp->wq_name),
			 "compirq/%u-%s", irq, name);
		comp->wq = create_singlethread_workqueue(comp->wq_name);
		if (!comp->wq) {
			printk(KERN_ERR "Failed to create compat-threaded-IRQ workqueue %s\n",
			       comp->wq_name);
			return -ENOMEM;
		}
	}
	return request_irq(irq, compat_irq_dispatcher, flags, name, comp);
}

static inline
void compat_free_threaded_irq(struct compat_threaded_irq *comp)
{
	free_irq(comp->irq, comp);
}

static inline
void compat_destroy_threaded_irq(struct compat_threaded_irq *comp)
{
	if (comp->wq)
		destroy_workqueue(comp->wq);
	comp->wq = NULL;
}

static inline
void compat_synchronize_threaded_irq(struct compat_threaded_irq *comp)
{
	synchronize_irq(comp->irq);
	cancel_work_sync(&comp->work);
}

/**
 * list_entry_rcu - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 *
 * This primitive may safely run concurrently with the _rcu list-mutation
 * primitives such as list_add_rcu() as long as it's guarded by rcu_read_lock().
 */
#define list_entry_rcu(ptr, type, member) \
	container_of(rcu_dereference(ptr), type, member)

#define skb_walk_frags(skb, iter)	\
	for (iter = skb_shinfo(skb)->frag_list; iter; iter = iter->next)

#ifndef CONFIG_64BIT

typedef struct {
	long long counter;
} atomic64_t;

#define atomic64_read LINUX_BACKPORT(atomic64_read)
extern long long atomic64_read(const atomic64_t *v);
#define atomic64_add_return LINUX_BACKPORT(atomic64_add_return)
extern long long atomic64_add_return(long long a, atomic64_t *v);

#define atomic64_inc_return(v)          atomic64_add_return(1LL, (v))

#endif

/**
 * sk_rmem_alloc_get - returns read allocations
 * @sk: socket
 *
 * Returns sk_rmem_alloc
 */
static inline int sk_rmem_alloc_get(const struct sock *sk)
{
	return atomic_read(&sk->sk_rmem_alloc);
}

/**
 * sk_wmem_alloc_get - returns write allocations
 * @sk: socket
 *
 * Returns sk_wmem_alloc minus initial offset of one
 */
static inline int sk_wmem_alloc_get(const struct sock *sk)
{
	return atomic_read(&sk->sk_wmem_alloc) - 1;
}

/**
 * sk_has_allocations - check if allocations are outstanding
 * @sk: socket
 *
 * Returns true if socket has write or read allocations
 */
static inline bool sk_has_allocations(const struct sock *sk)
{
	return sk_wmem_alloc_get(sk) || sk_rmem_alloc_get(sk);
}

#define USB_SUBCLASS_VENDOR_SPEC	0xff

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)) */

#endif /* LINUX_26_31_COMPAT_H */
