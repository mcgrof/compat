/*
 * Copyright 2007	Luis R. Rodriguez <mcgrof@winlab.rutgers.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.28.
 */

#include <linux/compat.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))

#include <linux/usb.h>

/* 2.6.28 compat code goes here */

/*
 * Compat-wireless notes for USB backport stuff:
 *
 * urb->reject exists on 2.6.27, the poison/unpoison helpers
 * did not though. The anchor poison does not exist so we cannot use them.
 *
 * USB anchor poising seems to exist to prevent future driver sumbissions
 * of usb_anchor_urb() to an anchor marked as poisoned. For older kernels
 * we cannot use that, so new usb_anchor_urb()s will be anchored. The down
 * side to this should be submission of URBs will continue being anchored
 * on an anchor instead of having them being rejected immediately when the
 * driver realized we needed to stop. For ar9170 we poison URBs upon the
 * ar9170 mac80211 stop callback(), don't think this should be so bad.
 * It mean there is period of time in older kernels for which we continue
 * to anchor new URBs to a known stopped anchor. We have two anchors
 * (TX, and RX)
 */

#if 0
/**
 * usb_poison_urb - reliably kill a transfer and prevent further use of an URB
 * @urb: pointer to URB describing a previously submitted request,
 *	may be NULL
 *
 * This routine cancels an in-progress request.  It is guaranteed that
 * upon return all completion handlers will have finished and the URB
 * will be totally idle and cannot be reused.  These features make
 * this an ideal way to stop I/O in a disconnect() callback.
 * If the request has not already finished or been unlinked
 * the completion handler will see urb->status == -ENOENT.
 *
 * After and while the routine runs, attempts to resubmit the URB will fail
 * with error -EPERM.  Thus even if the URB's completion handler always
 * tries to resubmit, it will not succeed and the URB will become idle.
 *
 * This routine may not be used in an interrupt context (such as a bottom
 * half or a completion handler), or when holding a spinlock, or in other
 * situations where the caller can't schedule().
 *
 * This routine should not be called by a driver after its disconnect
 * method has returned.
 */
void usb_poison_urb(struct urb *urb)
{
	might_sleep();
	if (!(urb && urb->dev && urb->ep))
		return;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	spin_lock_irq(&usb_reject_lock);
#endif
	++urb->reject;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	spin_unlock_irq(&usb_reject_lock);
#endif
	/*
	 * XXX: usb_hcd_unlink_urb() needs backporting... this is defined
	 * on usb hcd.c but urb.c gets access to it. That is, older kernels
	 * have usb_hcd_unlink_urb() but its not exported, nor can we
	 * re-implement it exactly. This essentially dequeues the urb from
	 * hw, we need to figure out a way to backport this.
	 */
	//usb_hcd_unlink_urb(urb, -ENOENT);

	wait_event(usb_kill_urb_queue, atomic_read(&urb->use_count) == 0);
}
EXPORT_SYMBOL_GPL(usb_poison_urb);
#endif

#include <pcmcia/ds.h>
struct pcmcia_cfg_mem {
	tuple_t tuple;
	cisparse_t parse;
	u8 buf[256];
	cistpl_cftable_entry_t dflt;
};
/**
 * pcmcia_loop_config() - loop over configuration options
 * @p_dev:	the struct pcmcia_device which we need to loop for.
 * @conf_check:	function to call for each configuration option.
 *		It gets passed the struct pcmcia_device, the CIS data
 *		describing the configuration option, and private data
 *		being passed to pcmcia_loop_config()
 * @priv_data:	private data to be passed to the conf_check function.
 *
 * pcmcia_loop_config() loops over all configuration options, and calls
 * the driver-specific conf_check() for each one, checking whether
 * it is a valid one. Returns 0 on success or errorcode otherwise.
 */
int pcmcia_loop_config(struct pcmcia_device *p_dev,
		       int	(*conf_check)	(struct pcmcia_device *p_dev,
						 cistpl_cftable_entry_t *cfg,
						 cistpl_cftable_entry_t *dflt,
						 unsigned int vcc,
						 void *priv_data),
		       void *priv_data)
{
	struct pcmcia_cfg_mem *cfg_mem;

	tuple_t *tuple;
	int ret;
	unsigned int vcc;

	cfg_mem = kzalloc(sizeof(struct pcmcia_cfg_mem), GFP_KERNEL);
	if (cfg_mem == NULL)
		return -ENOMEM;

	/* get the current Vcc setting */
	vcc = p_dev->socket->socket.Vcc;

	tuple = &cfg_mem->tuple;
	tuple->TupleData = cfg_mem->buf;
	tuple->TupleDataMax = 255;
	tuple->TupleOffset = 0;
	tuple->DesiredTuple = CISTPL_CFTABLE_ENTRY;
	tuple->Attributes = 0;

	ret = pcmcia_get_first_tuple(p_dev, tuple);
	while (!ret) {
		cistpl_cftable_entry_t *cfg = &cfg_mem->parse.cftable_entry;

		if (pcmcia_get_tuple_data(p_dev, tuple))
			goto next_entry;

		if (pcmcia_parse_tuple(tuple, &cfg_mem->parse))
			goto next_entry;

		/* default values */
		p_dev->conf.ConfigIndex = cfg->index;
		if (cfg->flags & CISTPL_CFTABLE_DEFAULT)
			cfg_mem->dflt = *cfg;

		ret = conf_check(p_dev, cfg, &cfg_mem->dflt, vcc, priv_data);
		if (!ret)
			break;

next_entry:
		ret = pcmcia_get_next_tuple(p_dev, tuple);
	}

	return ret;
}
EXPORT_SYMBOL(pcmcia_loop_config);

void usb_unpoison_urb(struct urb *urb)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	unsigned long flags;
#endif

	if (!urb)
		return;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	spin_lock_irqsave(&usb_reject_lock, flags);
#endif
	--urb->reject;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	spin_unlock_irqrestore(&usb_reject_lock, flags);
#endif
}
EXPORT_SYMBOL_GPL(usb_unpoison_urb);


#if 0
/**
 * usb_poison_anchored_urbs - cease all traffic from an anchor
 * @anchor: anchor the requests are bound to
 *
 * this allows all outstanding URBs to be poisoned starting
 * from the back of the queue. Newly added URBs will also be
 * poisoned
 *
 * This routine should not be called by a driver after its disconnect
 * method has returned.
 */
void usb_poison_anchored_urbs(struct usb_anchor *anchor)
{
	struct urb *victim;

	spin_lock_irq(&anchor->lock);
	// anchor->poisoned = 1; /* XXX: Cannot backport */
	while (!list_empty(&anchor->urb_list)) {
		victim = list_entry(anchor->urb_list.prev, struct urb,
				    anchor_list);
		/* we must make sure the URB isn't freed before we kill it*/
		usb_get_urb(victim);
		spin_unlock_irq(&anchor->lock);
		/* this will unanchor the URB */
		usb_poison_urb(victim);
		usb_put_urb(victim);
		spin_lock_irq(&anchor->lock);
	}
	spin_unlock_irq(&anchor->lock);
}
EXPORT_SYMBOL_GPL(usb_poison_anchored_urbs);
#endif

/**
 * usb_get_from_anchor - get an anchor's oldest urb
 * @anchor: the anchor whose urb you want
 *
 * this will take the oldest urb from an anchor,
 * unanchor and return it
 */
struct urb *usb_get_from_anchor(struct usb_anchor *anchor)
{
	struct urb *victim;
	unsigned long flags;

	spin_lock_irqsave(&anchor->lock, flags);
	if (!list_empty(&anchor->urb_list)) {
		victim = list_entry(anchor->urb_list.next, struct urb,
				    anchor_list);
		usb_get_urb(victim);
		spin_unlock_irqrestore(&anchor->lock, flags);
		usb_unanchor_urb(victim);
	} else {
		spin_unlock_irqrestore(&anchor->lock, flags);
		victim = NULL;
	}

	return victim;
}

EXPORT_SYMBOL_GPL(usb_get_from_anchor);

/**
 * usb_scuttle_anchored_urbs - unanchor all an anchor's urbs
 * @anchor: the anchor whose urbs you want to unanchor
 *
 * use this to get rid of all an anchor's urbs
 */
void usb_scuttle_anchored_urbs(struct usb_anchor *anchor)
{
	struct urb *victim;
	unsigned long flags;

	spin_lock_irqsave(&anchor->lock, flags);
	while (!list_empty(&anchor->urb_list)) {
		victim = list_entry(anchor->urb_list.prev, struct urb,
				    anchor_list);
		usb_get_urb(victim);
		spin_unlock_irqrestore(&anchor->lock, flags);
		/* this may free the URB */
		usb_unanchor_urb(victim);
		usb_put_urb(victim);
		spin_lock_irqsave(&anchor->lock, flags);
	}
	spin_unlock_irqrestore(&anchor->lock, flags);
}

EXPORT_SYMBOL_GPL(usb_scuttle_anchored_urbs);

/**
 * usb_anchor_empty - is an anchor empty
 * @anchor: the anchor you want to query
 *
 * returns 1 if the anchor has no urbs associated with it
 */
int usb_anchor_empty(struct usb_anchor *anchor)
{
	return list_empty(&anchor->urb_list);
}

EXPORT_SYMBOL_GPL(usb_anchor_empty);


void __iomem *pci_ioremap_bar(struct pci_dev *pdev, int bar)
{
	/*
	 * Make sure the BAR is actually a memory resource, not an IO resource
	 */
	if (!(pci_resource_flags(pdev, bar) & IORESOURCE_MEM)) {
		WARN_ON(1);
		return NULL;
	}
	return ioremap_nocache(pci_resource_start(pdev, bar),
				     pci_resource_len(pdev, bar));
}
EXPORT_SYMBOL_GPL(pci_ioremap_bar);

static unsigned long round_jiffies_common(unsigned long j, int cpu,
		bool force_up)
{
	int rem;
	unsigned long original = j;

	/*
	 * We don't want all cpus firing their timers at once hitting the
	 * same lock or cachelines, so we skew each extra cpu with an extra
	 * 3 jiffies. This 3 jiffies came originally from the mm/ code which
	 * already did this.
	 * The skew is done by adding 3*cpunr, then round, then subtract this
	 * extra offset again.
	 */
	j += cpu * 3;

	rem = j % HZ;

	/*
	 * If the target jiffie is just after a whole second (which can happen
	 * due to delays of the timer irq, long irq off times etc etc) then
	 * we should round down to the whole second, not up. Use 1/4th second
	 * as cutoff for this rounding as an extreme upper bound for this.
	 * But never round down if @force_up is set.
	 */
	if (rem < HZ/4 && !force_up) /* round down */
		j = j - rem;
	else /* round up */
		j = j - rem + HZ;

	/* now that we have rounded, subtract the extra skew again */
	j -= cpu * 3;

	if (j <= jiffies) /* rounding ate our timeout entirely; */
		return original;
	return j;
}

/**
 * round_jiffies_up - function to round jiffies up to a full second
 * @j: the time in (absolute) jiffies that should be rounded
 *
 * This is the same as round_jiffies() except that it will never
 * round down.  This is useful for timeouts for which the exact time
 * of firing does not matter too much, as long as they don't fire too
 * early.
 */
unsigned long round_jiffies_up(unsigned long j)
{
	return round_jiffies_common(j, raw_smp_processor_id(), true);
}
EXPORT_SYMBOL_GPL(round_jiffies_up);

void skb_add_rx_frag(struct sk_buff *skb, int i, struct page *page, int off,
		int size)
{
	skb_fill_page_desc(skb, i, page, off, size);
	skb->len += size;
	skb->data_len += size;
	skb->truesize += size;
}
EXPORT_SYMBOL(skb_add_rx_frag);

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28) */
