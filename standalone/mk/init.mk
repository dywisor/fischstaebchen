INITRAMFS_COMPONENTS += init
NONOVERLAY_FEATURES  += init

PHONY += init
init: %: $(O)/init/bin/%

PHONY += install-init
install-init: install-%: init
	$(DOEXE) -- $(O)/init/bin/$* $(INITRAMFS)/$*
