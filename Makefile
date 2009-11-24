export KMODDIR?=       updates/
KMODDIR_ARG:=   "INSTALL_MOD_DIR=$(KMODDIR)"
ifneq ($(origin $(KLIB)), undefined)
KMODPATH_ARG:=  "INSTALL_MOD_PATH=$(KLIB)"
else
export KLIB:=          /lib/modules/$(shell uname -r)
endif
export KLIB_BUILD ?=    $(KLIB)/build

export PWD := $(shell pwd)

ifneq ($(wildcard $(KLIB_BUILD)/Makefile),)
COMPAT_LATEST_VERSION = 32
KERNEL_SUBLEVEL := $(shell $(MAKE) -C $(KLIB_BUILD) kernelversion | sed -n 's/^2\.6\.\([0-9]\+\).*/\1/p')
COMPAT_VERSIONS := $(shell I=$(COMPAT_LATEST_VERSION); while [ "$$I" -gt $(KERNEL_SUBLEVEL) ]; do echo $$I; I=$$(($$I - 1)); done)
$(foreach ver,$(COMPAT_VERSIONS),$(eval CONFIG_COMPAT_KERNEL_$(ver)=y))
endif

obj-m += compat.o
#compat-objs :=

compat-y += main.o

# Compat kernel compatibility code
compat-$(CONFIG_COMPAT_KERNEL_14) += compat-2.6.14.o
compat-$(CONFIG_COMPAT_KERNEL_18) += compat-2.6.18.o
compat-$(CONFIG_COMPAT_KERNEL_22) += compat-2.6.22.o
compat-$(CONFIG_COMPAT_KERNEL_23) += compat-2.6.23.o
compat-$(CONFIG_COMPAT_KERNEL_24) += compat-2.6.24.o
compat-$(CONFIG_COMPAT_KERNEL_25) += compat-2.6.25.o
compat-$(CONFIG_COMPAT_KERNEL_26) += compat-2.6.26.o
compat-$(CONFIG_COMPAT_KERNEL_27) += compat-2.6.27.o
compat-$(CONFIG_COMPAT_KERNEL_28) += compat-2.6.28.o
compat-$(CONFIG_COMPAT_KERNEL_29) += compat-2.6.29.o
compat-$(CONFIG_COMPAT_KERNEL_30) += compat-2.6.30.o
compat-$(CONFIG_COMPAT_KERNEL_31) += compat-2.6.31.o
compat-$(CONFIG_COMPAT_KERNEL_32) += compat-2.6.32.o

modules: 
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) modules 
install: modules
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) $(KMODDIR_ARG) $(KMODPATH_ARG) \
		modules_install
	depmod -a
clean:
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) clean
all: modules

clean-files := Module.symvers modules.order Module.markers
