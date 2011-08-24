/*
 * Copyright 2011    Hauke Mehrtens <hauke@hauke-m.de>
 * Copyright 2011    Alexey Dobriyan <adobriyan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Compatibility file for Linux wireless for kernels 3.0.
 */

#include <linux/compat.h>
#include <linux/if_ether.h>

int mac_pton(const char *s, u8 *mac)
{
	int i;

	/* XX:XX:XX:XX:XX:XX */
	if (strlen(s) < 3 * ETH_ALEN - 1)
		return 0;

	/* Don't dirty result unless string is valid MAC. */
	for (i = 0; i < ETH_ALEN; i++) {
		if (!strchr("0123456789abcdefABCDEF", s[i * 3]))
			return 0;
		if (!strchr("0123456789abcdefABCDEF", s[i * 3 + 1]))
			return 0;
		if (i != ETH_ALEN - 1 && s[i * 3 + 2] != ':')
			return 0;
	}
	for (i = 0; i < ETH_ALEN; i++) {
		mac[i] = (hex_to_bin(s[i * 3]) << 4) | hex_to_bin(s[i * 3 + 1]);
	}
	return 1;
}
EXPORT_SYMBOL(mac_pton);
