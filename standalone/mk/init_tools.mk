INIT_TOOLS += filescan
INIT_TOOLS += filesize
INIT_TOOLS += untar


INIT_TOOLS += ztmpfs
INITRAMFS_COMPONENTS += zdisk
NONOVERLAY_FEATURES  += zdisk

PHONY += zdisk
zdisk: ztmpfs

PHONY += install-zdisk
install-zdisk: install-%: %
	$(INSTALL) -d -m 0755 $(INITRAMFS)/bin
	ln -f -s -- ztmpfs $(INITRAMFS)/bin/$*
	ln -f -s -- ztmpfs $(INITRAMFS)/bin/mount.$*


define ITOOL_INSTALL_ztmpfs
	ln -f -s -- ztmpfs $(INITRAMFS)/bin/mount.$(1)
endef

$(foreach p,$(INIT_TOOLS),$(eval $(call INIT_TOOL_DEF,$(p))))
