ifeq ($(WITH_BROKEN),1)

ifneq ($(filter $(INITRAMFS_OVERLAYS),union_mount_base)),)
INITRAMFS_COMPONENTS += aufs-overlayfs
NONOVERLAY_FEATURES  += aufs-overlayfs
endif

PHONY += aufs-overlayfs
aufs-overlayfs: $(O)/init/bin/aufs-overlayfs-wrapper

PHONY += install-aufs-overlayfs
install-aufs-overlayfs: install-%: %
	$(DOEXE) -- $(O)/init/bin/aufs-overlayfs-wrapper \
		$(INITRAMFS)/bin/aufs-overlayfs-genscript


endif
