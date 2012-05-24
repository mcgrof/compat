#ifndef LINUX_3_5_COMPAT_H
#define LINUX_3_5_COMPAT_H

#include <linux/version.h>
#include <linux/fs.h>
#include <linux/etherdevice.h>
#include <linux/net.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))

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
