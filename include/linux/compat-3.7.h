#ifndef LINUX_3_7_COMPAT_H
#define LINUX_3_7_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))

#include <linux/workqueue.h>
#include <linux/tty.h>

bool mod_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork,
		      unsigned long delay);

/* Backports tty_lock: Localise the lock */
#define tty_lock(__tty) tty_lock()
#define tty_unlock(__tty) tty_unlock()

#define tty_port_register_device(port, driver, index, device) \
	tty_register_device(driver, index, device)

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)) */

#endif /* LINUX_3_7_COMPAT_H */
