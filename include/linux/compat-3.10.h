#ifndef LINUX_3_10_COMPAT_H
#define LINUX_3_10_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0))

#include <linux/scatterlist.h>
#include <linux/mm.h>
#include <linux/fb.h>

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

/*
 * This is a linux-next data structure element collateral evolution,
 * we use a wrapper to avoid #ifdef hell to backport it. This allows
 * us to use a simple fb_info_skip_vt_switch() replacement for when
 * the new data structure element is used. If coccinelle SmPL grammar
 * could be used to express the transformation for us on compat-drivers
 * it means we'd need to express it only once. If the structure element
 * collateral evolution were to be used *at development* time and we'd
 * have a way to express the inverse through SmPL we'd be able to
 * backport this collateral evolution automatically for any new driver
 * that used it. We'd use coccinelle to look for it and do the
 * transformations for us based on the original commit (maybe SmPL
 * would be listed on the commit log.
 *
 * We may need the LINUX_BACKPORT() call that adds the backport_
 * prefix for older kernels than 3.10 if distros decide to
 * add this same static inline themselves (although unlikely).
 */
#define fb_enable_skip_vt_switch LINUX_BACKPORT(fb_enable_skip_vt_switch)
static inline void fb_enable_skip_vt_switch(struct fb_info *info)
{
}

/**
 * backport of:
 *
 * commit 6ed7ffddcf61f668114edb676417e5fb33773b59
 * Author: H Hartley Sweeten <hsweeten@visionengravers.com>
 * Date:   Wed Mar 6 11:24:44 2013 -0700
 *
 *     pcmcia/ds.h: introduce helper for pcmcia_driver module boilerplate
 */

/**
 * module_pcmcia_driver() - Helper macro for registering a pcmcia driver
 * @__pcmcia_driver: pcmcia_driver struct
 *
 * Helper macro for pcmcia drivers which do not do anything special in module
 * init/exit. This eliminates a lot of boilerplate. Each module may only use
 * this macro once, and calling it replaces module_init() and module_exit().
 */
#define module_pcmcia_driver(__pcmcia_driver) \
	module_driver(__pcmcia_driver, pcmcia_register_driver, \
			pcmcia_unregister_driver)

#else /* kernel is >= 3.10 */
/*
 * We'd delete this upstream ever got this, we use our
 * backport_ prefix with LINUX_BACKPORT() so that if this
 * does get upstream we would not have to add another ifdef
 * here for the kernels in between v3.10.. up to the point
 * the routine would have gotten added, we'd just delete this
 * #else condition completely. If we didn't have this and
 * say 3.12 added the static inline upstream, we'd have a
 * clash on the backport for 3.12 as the routine would
 * already be defined *but* we'd need it for 3.11.
 */
#define fb_enable_skip_vt_switch LINUX_BACKPORT(fb_enable_skip_vt_switch)
static inline void fb_enable_skip_vt_switch(struct fb_info *info)
{
	info->skip_vt_switch = true;
}
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)) */

#endif /* LINUX_3_10_COMPAT_H */
