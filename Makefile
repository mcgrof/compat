export KMODDIR?=       updates/
KMODDIR_ARG:=   "INSTALL_MOD_DIR=$(KMODDIR)"
ifneq ($(origin $(KLIB)), undefined)
KMODPATH_ARG:=  "INSTALL_MOD_PATH=$(KLIB)"
else
export KLIB:=          /lib/modules/$(shell uname -r)
endif
export KLIB_BUILD ?=    $(KLIB)/build

export PWD := $(shell pwd)

ifeq ($(KERNELRELEASE),)
export COMPAT_BASE_TREE := "linux-next.git"
# For this specific tree this is only relevant in
# terms of the last time we synched code up with upstream
# for internal stuff. For other compatability projects this
# would be the git describe of the base tree you are
# working with.
export COMPAT_BASE_TREE_VERSION := "next-20100517"
export COMPAT_VERSION := $(shell git describe)
endif

# This generates a bunch of CONFIG_COMPAT_KERNEL_2_6_22 CONFIG_COMPAT_KERNEL_3_0 .. etc for
# each kernel release you need an object for.
ifneq ($(wildcard $(KLIB_BUILD)/Makefile),)

COMPAT_LATEST_VERSION = 3
KERNEL_VERSION := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^\([0-9]\)\..*/\1/p')

ifneq ($(KERNEL_VERSION),2)
KERNEL_SUBLEVEL := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^3\.\([0-9]\+\).*/\1/p')
else
COMPAT_26LATEST_VERSION = 39
KERNEL_26SUBLEVEL := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^2\.6\.\([0-9]\+\).*/\1/p')
COMPAT_26VERSIONS := $(shell I=$(COMPAT_26LATEST_VERSION); while [ "$$I" -gt $(KERNEL_26SUBLEVEL) ]; do echo $$I; I=$$(($$I - 1)); done)
$(foreach ver,$(COMPAT_26VERSIONS),$(eval export CONFIG_COMPAT_KERNEL_2_6_$(ver)=y))
KERNEL_SUBLEVEL := -1
endif

COMPAT_VERSIONS := $(shell I=$(COMPAT_LATEST_VERSION); while [ "$$I" -gt $(KERNEL_SUBLEVEL) ]; do echo $$I; I=$$(($$I - 1)); done)
$(foreach ver,$(COMPAT_VERSIONS),$(eval export CONFIG_COMPAT_KERNEL_3_$(ver)=y))

endif

ifeq ($(CONFIG_COMPAT_KERNEL_2_6_33),y)
ifneq ($(CONFIG_FW_LOADER),)
 export CONFIG_COMPAT_FIRMWARE_CLASS=m
endif
endif

ifeq ($(CONFIG_COMPAT_KERNEL_2_6_36),y)
 export CONFIG_COMPAT_KFIFO=m
endif #CONFIG_COMPAT_KERNEL_2_6_36

obj-y += compat/

# This hack lets us put our include path first than the kernel's
# when building our compat modules. Your own makefile would look
# the same.
NOSTDINC_FLAGS := -I$(M)/include/ \
	-include $(M)/include/linux/compat-2.6.h \
	$(CFLAGS) \
	-DCOMPAT_BASE_TREE="\"$(COMPAT_BASE_TREE)\"" \
	-DCOMPAT_BASE_TREE_VERSION="\"$(COMPAT_BASE_TREE_VERSION)\"" \
	-DCOMPAT_PROJECT="\"Generic kernel\"" \
	-DCOMPAT_VERSION="\"$(COMPAT_VERSION)\""

modules:
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) modules
install: modules
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) $(KMODDIR_ARG) $(KMODPATH_ARG) \
		modules_install
	depmod -a
	@./scripts/compat_firmware_install
clean:
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) clean
all: modules

clean-files := Module.symvers modules.order Module.markers compat/modules.order
