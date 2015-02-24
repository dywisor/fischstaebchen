INITRAMFS_COMPONENTS += overlay

INITRAMFS_OVERLAYS += base misc
INITRAMFS_OVERLAYS += phaseout
INITRAMFS_OVERLAYS += separate_usr squashed_rootfs
INITRAMFS_OVERLAYS += union_mount_base union_mount
INITRAMFS_OVERLAYS += stagedive
#INITRAMFS_OVERLAYS += stagedive_bootstrap_arch
INITRAMFS_OVERLAYS += stagedive_bootstrap_gentoo
#INITRAMFS_OVERLAYS += dotfiles
INITRAMFS_OVERLAYS += newroot_hooks
INITRAMFS_OVERLAYS += standalone
INITRAMFS_OVERLAYS += telinit udhcpc
INITRAMFS_OVERLAYS += $(OVERLAYS)
INITRAMFS_OVERLAYS += $(filter-out $(NONOVERLAY_FEATURES),$(FEATURES))

INITRAMFS_OVERLAY_SRC = $(__PRJROOT)/overlay

OVERLAY_O           = $(O)/overlay
OVERLAY_O_FAKEROOTD = $(O)/fakeroot-setup.d

OVERLAY_MAKEOPTS  = -C $(INITRAMFS_OVERLAY_SRC)
OVERLAY_MAKEOPTS += O='$(O)' OVERLAY_O='$(OVERLAY_O)'
OVERLAY_MAKEOPTS += OVERLAY_O_FAKEROOTD='$(OVERLAY_O_FAKEROOTD)'
OVERLAY_MAKEOPTS += OVERLAYS='$(INITRAMFS_OVERLAYS)'



$(OVERLAY_O): $(INITRAMFS_OVERLAY_SRC)/Makefile
	$(MAKE) $(OVERLAY_MAKEOPTS)

PHONY += overlay
overlay: $(OVERLAY_O)


PHONY += install-overlay
install-overlay: overlay
	$(MAKE) $(OVERLAY_MAKEOPTS) DESTDIR=$(INITRAMFS) install

PHONY += uninstall-overlay
uninstall-overlay:
	test -n '$(INITRAMFS)'

	test ! -d '$(OVERLAY_O)' || \
	cd $(OVERLAY_O) && find ./ -type f -print0 | \
		xargs -r -0 -I '{R}' rm -vf -- '$(INITRAMFS)/{R}'

PHONY += reinstall-overlay
reinstall-overlay: uninstall-overlay
	$(MAKE) $(OVERLAY_MAKEOPTS) DESTDIR=$(INITRAMFS) install
