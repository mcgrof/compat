#ifndef LINUX_3_9_COMPAT_H
#define LINUX_3_9_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0))

#include <linux/idr.h>
#include <linux/list.h>
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,25))
#include <linux/rculist.h>
#endif
#include <net/sock.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/printk.h>
#include <linux/scatterlist.h>

/* include this before changing hlist_for_each_* to use the old versions. */
#include <net/sch_generic.h>

/* Lets expect distributions might backport this */

#ifndef for_each_sg_page
/*
 * sg page iterator
 *
 * Iterates over sg entries page-by-page.  On each successful iteration,
 * @piter->page points to the current page, @piter->sg to the sg holding this
 * page and @piter->sg_pgoffset to the page's page offset within the sg. The
 * iteration will stop either when a maximum number of sg entries was reached
 * or a terminating sg (sg_last(sg) == true) was reached.
 */
struct sg_page_iter {
	struct page		*page;		/* current page */
	struct scatterlist	*sg;		/* sg holding the page */
	unsigned int		sg_pgoffset;	/* page offset within the sg */

	/* these are internal states, keep away */
	unsigned int		__nents;	/* remaining sg entries */
	int			__pg_advance;	/* nr pages to advance at the
						 * next step */
};

#define __sg_page_iter_next LINUX_BACKPORT(__sg_page_iter_next)
bool __sg_page_iter_next(struct sg_page_iter *piter);
#define __sg_page_iter_start LINUX_BACKPORT(__sg_page_iter_start)
void __sg_page_iter_start(struct sg_page_iter *piter,
			  struct scatterlist *sglist, unsigned int nents,
			  unsigned long pgoffset);

/**
 * for_each_sg_page - iterate over the pages of the given sg list
 * @sglist:	sglist to iterate over
 * @piter:	page iterator to hold current page, sg, sg_pgoffset
 * @nents:	maximum number of sg entries to iterate over
 * @pgoffset:	starting page offset
 */
#define for_each_sg_page(sglist, piter, nents, pgoffset)		   \
	for (__sg_page_iter_start((piter), (sglist), (nents), (pgoffset)); \
	     __sg_page_iter_next(piter);)

#endif /* for_each_sg_page assumption */

/* backports 7a555613 */
#if defined(CONFIG_DYNAMIC_DEBUG)
#define dynamic_hex_dump(prefix_str, prefix_type, rowsize,     \
			 groupsize, buf, len, ascii)            \
do {                                                           \
	DEFINE_DYNAMIC_DEBUG_METADATA(descriptor,               \
	__builtin_constant_p(prefix_str) ? prefix_str : "hexdump");\
	if (unlikely(descriptor.flags & _DPRINTK_FLAGS_PRINT))  \
		print_hex_dump(KERN_DEBUG, prefix_str,          \
			       prefix_type, rowsize, groupsize, \
			       buf, len, ascii);                \
} while (0)
#define print_hex_dump_debug(prefix_str, prefix_type, rowsize, \
			     groupsize, buf, len, ascii)        \
	dynamic_hex_dump(prefix_str, prefix_type, rowsize,      \
			 groupsize, buf, len, ascii)
#else
#define print_hex_dump_debug(prefix_str, prefix_type, rowsize,         \
			     groupsize, buf, len, ascii)                \
	print_hex_dump(KERN_DEBUG, prefix_str, prefix_type, rowsize,    \
		       groupsize, buf, len, ascii)
#endif /* defined(CONFIG_DYNAMIC_DEBUG) */


/**
 * backport of idr idr_alloc() usage
 * 
 * This backports a patch series send by Tejun Heo:
 * https://lkml.org/lkml/2013/2/2/159
 */
static inline void compat_idr_destroy(struct idr *idp)
{
	idr_remove_all(idp);
	idr_destroy(idp);
}
#define idr_destroy(idp) compat_idr_destroy(idp)

static inline int idr_alloc(struct idr *idr, void *ptr, int start, int end,
			    gfp_t gfp_mask)
{
	int id, ret;

	do {
		if (!idr_pre_get(idr, gfp_mask))
			return -ENOMEM;
		ret = idr_get_new_above(idr, ptr, start, &id);
		if (!ret && id > end) {
			idr_remove(idr, id);
			ret = -ENOSPC;
		}
	} while (ret == -EAGAIN);

	return ret ? ret : id;
}

static inline void idr_preload(gfp_t gfp_mask)
{
}

static inline void idr_preload_end(void)
{
}


/**
 * backport:
 *
 * commit 0bbacca7c3911451cea923b0ad6389d58e3d9ce9
 * Author: Sasha Levin <sasha.levin@oracle.com>
 * Date:   Thu Feb 7 12:32:18 2013 +1100
 *
 *     hlist: drop the node parameter from iterators
 */

#define hlist_entry_safe(ptr, type, member) \
	(ptr) ? hlist_entry(ptr, type, member) : NULL

#undef hlist_for_each_entry
/**
 * hlist_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, member)					\
	for (pos = hlist_entry_safe((head)->first, typeof(*(pos)), member);	\
	     pos;								\
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#undef hlist_for_each_entry_safe
/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(pos, n, head, member) 			\
	for (pos = hlist_entry_safe((head)->first, typeof(*pos), member);	\
	     pos && ({ n = pos->member.next; 1; });				\
	     pos = hlist_entry_safe(n, typeof(*pos), member))

#undef hlist_for_each_entry_rcu
/**
 * hlist_for_each_entry_rcu - iterate over rcu list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 *
 * This list-traversal primitive may safely run concurrently with
 * the _rcu list-mutation primitives such as hlist_add_head_rcu()
 * as long as the traversal is guarded by rcu_read_lock().
 */
#define hlist_for_each_entry_rcu(pos, head, member)				\
	for (pos = hlist_entry_safe (rcu_dereference_raw(hlist_first_rcu(head)),\
			typeof(*(pos)), member);				\
		pos;								\
		pos = hlist_entry_safe(rcu_dereference_raw(hlist_next_rcu(	\
			&(pos)->member)), typeof(*(pos)), member))

#undef sk_for_each
#define sk_for_each(__sk, list) \
	hlist_for_each_entry(__sk, list, sk_node)

#define tty_flip_buffer_push(port) tty_flip_buffer_push((port)->tty)
#define tty_insert_flip_string(port, chars, size) tty_insert_flip_string((port)->tty, chars, size)

/**
 * backport of:
 *
 * commit 496ad9aa8ef448058e36ca7a787c61f2e63f0f54
 * Author: Al Viro <viro@zeniv.linux.org.uk>
 * Date:   Wed Jan 23 17:07:38 2013 -0500
 *
 *     new helper: file_inode(file)
 */
static inline struct inode *file_inode(struct file *f)
{
	return f->f_path.dentry->d_inode;
}

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,9,0)) */

#endif /* LINUX_3_9_COMPAT_H */
