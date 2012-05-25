#ifndef LINUX_3_5_COMPAT_H
#define LINUX_3_5_COMPAT_H

#include <linux/version.h>
#include <linux/fs.h>
#include <linux/etherdevice.h>
#include <linux/net.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))

#include <linux/pkt_sched.h>

/*
 * This backports:
 *
 *   From 76e3cc126bb223013a6b9a0e2a51238d1ef2e409 Mon Sep 17 00:00:00 2001
 *   From: Eric Dumazet <edumazet@google.com>
 *   Date: Thu, 10 May 2012 07:51:25 +0000
 *   Subject: [PATCH] codel: Controlled Delay AQM
 */

/* CODEL */

enum {
	TCA_CODEL_UNSPEC,
	TCA_CODEL_TARGET,
	TCA_CODEL_LIMIT,
	TCA_CODEL_INTERVAL,
	TCA_CODEL_ECN,
	__TCA_CODEL_MAX
};

#define TCA_CODEL_MAX	(__TCA_CODEL_MAX - 1)

struct tc_codel_xstats {
	__u32	maxpacket; /* largest packet we've seen so far */
	__u32	count;	   /* how many drops we've done since the last time we
			    * entered dropping state
			    */
	__u32	lastcount; /* count at entry to dropping state */
	__u32	ldelay;    /* in-queue delay seen by most recently dequeued packet */
	__s32	drop_next; /* time to drop next packet */
	__u32	drop_overlimit; /* number of time max qdisc packet limit was hit */
	__u32	ecn_mark;  /* number of packets we ECN marked instead of dropped */
	__u32	dropping;  /* are we in dropping state ? */
};

/* Backports tty_lock: Localise the lock */
#define tty_lock(__tty) tty_lock()
#define tty_unlock(__tty) tty_unlock()

/* Backport ether_addr_equal */
static inline bool ether_addr_equal(const u8 *addr1, const u8 *addr2)
{
    return !compare_ether_addr(addr1, addr2);
}

#define net_ratelimited_function(function, ...)			\
do {								\
	if (net_ratelimit())					\
		function(__VA_ARGS__);				\
} while (0)

#define net_emerg_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_emerg, fmt, ##__VA_ARGS__)
#define net_alert_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_alert, fmt, ##__VA_ARGS__)
#define net_crit_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_crit, fmt, ##__VA_ARGS__)
#define net_err_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_err, fmt, ##__VA_ARGS__)
#define net_notice_ratelimited(fmt, ...)			\
	net_ratelimited_function(pr_notice, fmt, ##__VA_ARGS__)
#define net_warn_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_warn, fmt, ##__VA_ARGS__)
#define net_info_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_info, fmt, ##__VA_ARGS__)
#define net_dbg_ratelimited(fmt, ...)				\
	net_ratelimited_function(pr_debug, fmt, ##__VA_ARGS__)

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0)) */

#endif /* LINUX_3_5_COMPAT_H */
