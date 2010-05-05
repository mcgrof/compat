#ifndef LINUX_26_35_COMPAT_H
#define LINUX_26_35_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
#include <linux/etherdevice.h>
#include <net/sock.h>

#define IW_HANDLER(id, func)			\
	[IW_IOCTL_IDX(id)] = func

#define  SDIO_BUS_ECSI		0x20	/* Enable continuous SPI interrupt */
#define  SDIO_BUS_SCSI		0x40	/* Support continuous SPI interrupt */

#define netdev_hw_addr dev_mc_list

static inline wait_queue_head_t *sk_sleep(struct sock *sk)
{
	return sk->sk_sleep;
}

#define usb_alloc_coherent(dev, size, mem_flags, dma) usb_buffer_alloc(dev, size, mem_flags, dma)
#define usb_free_coherent(dev, size, addr, dma) usb_buffer_free(dev, size, addr, dma)

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)) */

#endif /* LINUX_26_35_COMPAT_H */