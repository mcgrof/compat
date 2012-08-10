#ifndef LINUX_3_7_COMPAT_H
#define LINUX_3_7_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))

#include <linux/workqueue.h>

bool mod_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork,
		      unsigned long delay);

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)) */

#endif /* LINUX_3_7_COMPAT_H */
