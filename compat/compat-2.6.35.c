/*
 * Copyright 2010    Kshitij Kulshreshtha <kkhere.geo@gmail.com>
 * Copyright 2012    Luis R. Rodriguez <mcgrof@do-not-panic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.35.
 */

#include <linux/compat.h>
#include <linux/ctype.h>

#ifdef CONFIG_RPS
int netif_set_real_num_rx_queues(struct net_device *dev, unsigned int rxq)
{
	int rc;

	/* we can't update the sysfs object for older kernels */
	if (dev->reg_state == NETREG_REGISTERED)
		return -EINVAL;
	dev->num_rx_queues = rxq;
	return 0;
}
#endif

/*
 * Routine to help set real_num_tx_queues. To avoid skbs mapped to queues
 * greater then real_num_tx_queues stale skbs on the qdisc must be flushed.
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
int netif_set_real_num_tx_queues(struct net_device *dev, unsigned int txq)
{
	unsigned int real_num = dev->real_num_tx_queues;

	if (unlikely(txq > dev->num_tx_queues))
		return -EINVAL;
	else if (txq > real_num)
		dev->real_num_tx_queues = txq;
	else if (txq < real_num) {
		dev->real_num_tx_queues = txq;
		qdisc_reset_all_tx_gt(dev, txq);
	}
	return 0;
}
#else
int netif_set_real_num_tx_queues(struct net_device *dev, unsigned int txq)
{
	dev->egress_subqueue_count = txq;
	/* XXX: consider qdisc reset for older kernels */
	return 0;
}
#endif
EXPORT_SYMBOL_GPL(netif_set_real_num_tx_queues);

/**
 * hex_to_bin - convert a hex digit to its real value
 * @ch: ascii character represents hex digit
 *
 * hex_to_bin() converts one hex digit to its actual value or -1 in case of bad
 * input.
 */
int compat_hex_to_bin(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}
EXPORT_SYMBOL_GPL(compat_hex_to_bin);

/**
 * noop_llseek - No Operation Performed llseek implementation
 * @file:	file structure to seek on
 * @offset:	file offset to seek to
 * @origin:	type of seek
 *
 * This is an implementation of ->llseek useable for the rare special case when
 * userspace expects the seek to succeed but the (device) file is actually not
 * able to perform the seek. In this case you use noop_llseek() instead of
 * falling back to the default implementation of ->llseek.
 */
loff_t noop_llseek(struct file *file, loff_t offset, int origin)
{
	return file->f_pos;
}
EXPORT_SYMBOL_GPL(noop_llseek);

