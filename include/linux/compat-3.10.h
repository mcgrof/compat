#ifndef LINUX_3_10_COMPAT_H
#define LINUX_3_10_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0))

#include <linux/scatterlist.h>
#include <linux/mm.h>

#define sg_page_iter_page LINUX_BACKPORT(sg_page_iter_page)
/**
 * sg_page_iter_page - get the current page held by the page iterator
 * @piter:     page iterator holding the page
 */
static inline struct page *sg_page_iter_page(struct sg_page_iter *piter)
{
	return nth_page(sg_page(piter->sg), piter->sg_pgoffset);
}

#define sg_page_iter_dma_address LINUX_BACKPORT(sg_page_iter_dma_address)
/**
 * sg_page_iter_dma_address - get the dma address of the current page held by
 * the page iterator.
 * @piter:     page iterator holding the page
 */
static inline dma_addr_t sg_page_iter_dma_address(struct sg_page_iter *piter)
{
	return sg_dma_address(piter->sg) + (piter->sg_pgoffset << PAGE_SHIFT);
}

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)) */

#endif /* LINUX_3_10_COMPAT_H */
