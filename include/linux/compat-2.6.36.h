#ifndef LINUX_26_36_COMPAT_H
#define LINUX_26_36_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))

#include <linux/usb.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>

#define kparam_block_sysfs_write(a)
#define kparam_unblock_sysfs_write(a)

struct va_format {
	const char *fmt;
	va_list *va;
};

#define device_rename(dev, new_name) device_rename(dev, (char *)new_name)

#ifdef CONFIG_COMPAT_USB_URB_THREAD_FIX
#define usb_scuttle_anchored_urbs(anchor)	compat_usb_scuttle_anchored_urbs(anchor)
#define usb_get_from_anchor(anchor)	compat_usb_get_from_anchor(anchor)
#define usb_unlink_anchored_urbs(anchor)	compat_usb_unlink_anchored_urbs(anchor)

extern void compat_usb_unlink_anchored_urbs(struct usb_anchor *anchor);
extern struct urb *compat_usb_get_from_anchor(struct usb_anchor *anchor);
extern void compat_usb_scuttle_anchored_urbs(struct usb_anchor *anchor);
#endif

/**
 * pcmcia_read_config_byte() - read a byte from a card configuration register
 *
 * pcmcia_read_config_byte() reads a byte from a configuration register in
 * attribute memory.
 */
static inline int pcmcia_read_config_byte(struct pcmcia_device *p_dev, off_t where, u8 *val)
{
        int ret;
        conf_reg_t reg = { 0, CS_READ, where, 0 };
        ret = pcmcia_access_configuration_register(p_dev, &reg);
        *val = reg.Value;
        return ret;
}

/**
 * pcmcia_write_config_byte() - write a byte to a card configuration register
 *
 * pcmcia_write_config_byte() writes a byte to a configuration register in
 * attribute memory.
 */
static inline int pcmcia_write_config_byte(struct pcmcia_device *p_dev, off_t where, u8 val)
{
	conf_reg_t reg = { 0, CS_WRITE, where, val };
	return pcmcia_access_configuration_register(p_dev, &reg);
}

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) */

#endif /* LINUX_26_36_COMPAT_H */
