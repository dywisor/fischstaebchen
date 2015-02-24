ifneq ($(filter fsck mkfs,$(FEATURES)),)
INITRAMFS_COMPONENTS += e2fsprogs
endif

E2FSPROGS_VERSION = 1.42.12
E2FSPROGS_SOURCE  = e2fsprogs-$(E2FSPROGS_VERSION)

E2FSPROGS_SRC_URI = https://www.kernel.org/pub/linux/kernel/people/tytso/e2fsprogs/v$(E2FSPROGS_VERSION)/$(E2FSPROGS_SOURCE).tar.xz

E2FSPROGS_SRC := $(DL_DIR)/$(notdir $(E2FSPROGS_SRC_URI))
E2FSPROGS_O   := $(O)/e2fsprogs/$(E2FSPROGS_SOURCE)

E2FSPROGS_FEATURES_ENABLE   =
E2FSPROGS_FEATURES_ENABLE  += fsck
E2FSPROGS_FEATURES_ENABLE  += libuuid
E2FSPROGS_FEATURES_ENABLE  += libblkid

E2FSPROGS_FEATURES_DISABLE  =
E2FSPROGS_FEATURES_DISABLE += testio-debug
E2FSPROGS_FEATURES_DISABLE += quota
E2FSPROGS_FEATURES_DISABLE += backtrace
E2FSPROGS_FEATURES_DISABLE += debugfs
E2FSPROGS_FEATURES_DISABLE += imager
E2FSPROGS_FEATURES_DISABLE += resizer
E2FSPROGS_FEATURES_DISABLE += defrag
E2FSPROGS_FEATURES_DISABLE += uuidd
E2FSPROGS_FEATURES_DISABLE +=

E2FSPROGS_FEATURES_WITH     =
E2FSPROGS_FEATURES_WITHOUT  =


PHONY += e2fsprogs
e2fsprogs: $(E2FSPROGS_O)/.buildstamp_all

PHONY += install-e2fsprogs
install-e2fsprogs: $(E2FSPROGS_O)/.buildstamp_all
ifneq ($(filter fsck,$(FEATURES)),)
	$(DOEXE) -- $(<D)/e2fsck/e2fsck $(INITRAMFS)/bin/e2fsck
	$(DOSYM) -- e2fsck $(INITRAMFS)/bin/fsck.ext2
	$(DOSYM) -- e2fsck $(INITRAMFS)/bin/fsck.ext3
	$(DOSYM) -- e2fsck $(INITRAMFS)/bin/fsck.ext4
endif
ifneq ($(filter mkfs,$(FEATURES)),)
	$(DOEXE) -- $(<D)/misc/mke2fs $(INITRAMFS)/bin/mke2fs
	$(DOSYM) -- mke2fs $(INITRAMFS)/bin/mkfs.ext2
	$(DOSYM) -- mke2fs $(INITRAMFS)/bin/mkfs.ext4
endif

$(E2FSPROGS_SRC):
	$(call F_FETCH_FILE,$(E2FSPROGS_SRC_URI),$(E2FSPROGS_SRC))

$(E2FSPROGS_O): $(E2FSPROGS_SRC) FORCE
	$(DO_DEFAULT_UNTAR)


$(E2FSPROGS_O)/.buildstamp_all: $(E2FSPROGS_O) FORCE
	cd $(@D) && \
		$(X_C_MAKEENV_STATIC) $(SHELL) ./configure \
			$(X_CONFIGURE_OPTS) \
			$(call F_FEATURE_CONFIGURE_OPTS,E2FSPROGS) \
			--with-root-prefix=/

	cd $(@D) && $(X_C_MAKEENV_STATIC) $(MAKE)

	$(CROSS_COMPILE)strip -s $(@D)/e2fsck/e2fsck
	$(CROSS_COMPILE)strip -s $(@D)/misc/mke2fs

	touch $@
