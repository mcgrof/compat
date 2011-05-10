#ifndef LINUX_26_40_COMPAT_H
#define LINUX_26_40_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,40))

#include <linux/rcupdate.h>

/*
 * This is not part of The 2.6.37 kernel yet but we
 * we use it to optimize the backport code we
 * need to implement. Instead of using ifdefs
 * to check what version of the check we use
 * we just replace all checks on current code
 * with this. I'll submit this upstream too, that
 * way all we'd have to do is to implement this
 * for older kernels, then we would not have to
 * edit the upstrema code for backport efforts.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
#define br_port_exists(dev)	(dev->priv_flags & IFF_BRIDGE_PORT)
#else
#define br_port_exists(dev)	(dev->br_port)
#endif

static __always_inline bool __is_kfree_rcu_offset(unsigned long offset)
{
	return offset < 4096;
}

static __always_inline
void __kfree_rcu(struct rcu_head *head, unsigned long offset)
{
	typedef void (*rcu_callback)(struct rcu_head *);

	/* This causes build failures with some kernel versions at positions
	 * where this methods is not used at all.
	 */
	/* BUILD_BUG_ON(!__builtin_constant_p(offset)); */

	/* See the kfree_rcu() header comment. */
	/* BUILD_BUG_ON(!__is_kfree_rcu_offset(offset)); */

	call_rcu(head, (rcu_callback)offset);
}

static inline void __rcu_reclaim(struct rcu_head *head)
{
	unsigned long offset = (unsigned long)head->func;

	if (__is_kfree_rcu_offset(offset))
		kfree((void *)head - offset);
	else
		head->func(head);
}

/**
 * kfree_rcu() - kfree an object after a grace period.
 * @ptr:	pointer to kfree
 * @rcu_head:	the name of the struct rcu_head within the type of @ptr.
 *
 * Many rcu callbacks functions just call kfree() on the base structure.
 * These functions are trivial, but their size adds up, and furthermore
 * when they are used in a kernel module, that module must invoke the
 * high-latency rcu_barrier() function at module-unload time.
 *
 * The kfree_rcu() function handles this issue.  Rather than encoding a
 * function address in the embedded rcu_head structure, kfree_rcu() instead
 * encodes the offset of the rcu_head structure within the base structure.
 * Because the functions are not allowed in the low-order 4096 bytes of
 * kernel virtual memory, offsets up to 4095 bytes can be accommodated.
 * If the offset is larger than 4095 bytes, a compile-time error will
 * be generated in __kfree_rcu().  If this error is triggered, you can
 * either fall back to use of call_rcu() or rearrange the structure to
 * position the rcu_head structure into the first 4096 bytes.
 *
 * Note that the allowable offset might decrease in the future, for example,
 * to allow something like kmem_cache_free_rcu().
 */
#define kfree_rcu(ptr, rcu_head)					\
	__kfree_rcu(&((ptr)->rcu_head), offsetof(typeof(*(ptr)), rcu_head))


#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,40)) */

#endif /* LINUX_26_40_COMPAT_H */
