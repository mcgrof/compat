export KMODDIR?=       updates/
KMODDIR_ARG:=   "INSTALL_MOD_DIR=$(KMODDIR)"
ifneq ($(origin $(KLIB)), undefined)
KMODPATH_ARG:=  "INSTALL_MOD_PATH=$(KLIB)"
else
export KLIB:=          /lib/modules/$(shell uname -r)
endif
export KLIB_BUILD ?=    $(KLIB)/build

export PWD := $(shell pwd)

# This generates a bunch of CONFIG_COMPAT_KERNEL_22 CONFIG_COMPAT_KERNEL_23 .. etc for
# each kernel release you need an object for.
ifneq ($(wildcard $(KLIB_BUILD)/Makefile),)
COMPAT_LATEST_VERSION = 32
KERNEL_SUBLEVEL := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^2\.6\.\([0-9]\+\).*/\1/p')
COMPAT_VERSIONS := $(shell I=$(COMPAT_LATEST_VERSION); while [ "$$I" -gt $(KERNEL_SUBLEVEL) ]; do echo $$I; I=$$(($$I - 1)); done)
$(foreach ver,$(COMPAT_VERSIONS),$(eval export CONFIG_COMPAT_KERNEL_$(ver)=y))
endif

ifeq ($(CONFIG_COMPAT_KERNEL_33),y)
 CONFIG_COMPAT_FIRMWARE_CLASS=m
 $(error "poo")
endif

obj-y += compat/

# This hack lets us put our include path first than the kernel's
# when building our compat modules. Your own makefile would look
# the same.
NOSTDINC_FLAGS := -I$(M)/include/ -include $(M)/include/linux/compat-2.6.h $(CFLAGS)

modules:
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) modules
install: modules
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) $(KMODDIR_ARG) $(KMODPATH_ARG) \
		modules_install
	depmod -a
clean:
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) clean
all: modules

clean-files := Module.symvers modules.order Module.markers compat/modules.order
