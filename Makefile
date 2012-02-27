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

# to check config and compat autoconf
export COMPAT_CONFIG=$(PWD)/.config
export COMPAT_AUTOCONF=$(PWD)/include/linux/compat_autoconf.h
export MAKE

else
# By stuffing this hear we avoid using
# this hackery on modpost, the 2nd section of module building.
#
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

endif

# Recursion lets us ensure we get this file included.
# Trick is to run make -C $(PWD) modules later.
-include $(COMPAT_CONFIG)

obj-y += compat/

all: $(COMPAT_CONFIG)

modules: $(COMPAT_CONFIG) $(COMPAT_AUTOCONF)
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) modules
	@touch modules

install: modules
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) $(KMODDIR_ARG) $(KMODPATH_ARG) \
		modules_install
	depmod -a

$(COMPAT_AUTOCONF): $(COMPAT_CONFIG)
	+@$(PWD)/scripts/gen-compat-autoconf.sh $(COMPAT_CONFIG) > $(COMPAT_AUTOCONF)

$(COMPAT_CONFIG):
	+@$(PWD)/scripts/gen-compat-config.sh > $(COMPAT_CONFIG)
	@$(MAKE) -C $(PWD) modules

install: modules

clean:
	$(MAKE) -C $(KLIB_BUILD) M=$(PWD) clean

clean-files := Module.symvers modules.order Module.markers compat/modules.order
clean-files += modules $(COMPAT_CONFIG) $(COMPAT_AUTOCONF)

.PHONY: all install clean
