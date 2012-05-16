#ifndef LINUX_3_5_COMPAT_H
#define LINUX_3_5_COMPAT_H

#include <linux/version.h>
#include <linux/fs.h>
#include <linux/etherdevice.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0))

extern int simple_open(struct inode *inode, struct file *file);

/* Backports tty_lock: Localise the lock */
#define tty_lock(__tty) tty_lock()
#define tty_unlock(__tty) tty_unlock()

/* Backport ether_addr_equal */
static inline bool ether_addr_equal(const u8 *addr1, const u8 *addr2)
{
    return !compare_ether_addr(addr1, addr2);
}

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0)) */

#endif /* LINUX_3_5_COMPAT_H */
