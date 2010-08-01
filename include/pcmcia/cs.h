#ifndef _COMPAT_PCMCIA_CS_H
#define _COMPAT_PCMCIA_CS_H
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
#include <pcmcia/cs_types.h>
#endif

#include_next <pcmcia/cs.h>
#endif
