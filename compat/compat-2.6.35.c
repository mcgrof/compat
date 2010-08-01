/*
 * Copyright 2010    Kshitij Kulshreshtha <kkhere.geo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 2.6.35.
 */

#include <linux/compat.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
#include <linux/ctype.h>

/**
 * hex_to_bin - convert a hex digit to its real value
 * @ch: ascii character represents hex digit
 *
 * hex_to_bin() converts one hex digit to its actual value or -1 in case of bad
 * input.
 */
int hex_to_bin(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}
EXPORT_SYMBOL(hex_to_bin);

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35) */
