INITRAMFS_COMPONENTS += busybox

X_GIT ?= git

BUSYBOX_SRC := $(S)/src/busybox
BUSYBOX_O   := $(O)/busybox
BUSYBOX_GIT_SRC := git://git.busybox.net/busybox

BUSYBOX_MAKEOPTS  = -C $(BUSYBOX_SRC) O=$(BUSYBOX_O)
BUSYBOX_MAKEOPTS += CROSS_COMPILE=$(CROSS_COMPILE)
BUSYBOX_MAKEOPTS += ARCH=$(ARCH)


PHONY += busybox-src
busybox-src: $(BUSYBOX_SRC)

PHONY += busybox-config
busybox-config: $(BUSYBOX_O)/.config

PHONY += install-busybox
install-busybox: busybox
	$(DOEXE) -- $(BUSYBOX_O)/busybox $(INITRAMFS)/bin/busybox
	ln -f -s -- busybox $(INITRAMFS)/bin/sh

# it's up to busybox' Makefile to decide whether a rebuild is necessary
PHONY += busybox
busybox: $(BUSYBOX_O)/busybox


$(BUSYBOX_SRC): FORCE
	mkdir -p -- $(@D)
	test -d $(@) || $(X_GIT) clone $(BUSYBOX_GIT_SRC) $(BUSYBOX_SRC)
	$(X_GIT) -C $(BUSYBOX_SRC) pull


$(BUSYBOX_O)/.config: $(FILESDIR)/busybox.config $(BUSYBOX_SRC) FORCE
	mkdir -p $(@D)
	$(MAKE) -s $(BUSYBOX_MAKEOPTS) defconfig 1>/dev/null

	cp -- $< $@.tmp
ifneq ($(CROSS_COMPILE),)
	sed -i -r \
		-e 's,^(CONFIG_CROSS_COMPILER_PREFIX=).*,\1"$(CROSS_COMPILE)",' \
		$@.tmp
endif
ifneq ($(SYSROOT),)
	sed -i -r \
		-e 's,^(CONFIG_SYSROOT=).*,\1"$(SYSROOT)",' \
		$@.tmp
endif

	mv -f -- $@.tmp $@

	yes '' | $(MAKE) -s $(BUSYBOX_MAKEOPTS) oldconfig 1>/dev/null


$(BUSYBOX_O)/busybox: $(BUSYBOX_SRC) $(BUSYBOX_O)/.config | $(BUSYBOX_SRC)
	$(MAKE) $(BUSYBOX_MAKEOPTS)
