UDHCPC_SCRIPT_FILE = /etc/udhcpc.script

VIRTUAL_OVERLAY_NAMES += telinit
$(O)/.stamp_overlay_telinit: $(FILESDIR)/telinit | _basedep_clean
	$(DOEXE) -- '$(<)' '$(OVERLAY_O:/=)/sbin/telinit'
	touch '$(@)'

VIRTUAL_OVERLAY_NAMES += udhcpc
$(O)/.stamp_overlay_udhcpc: $(FILESDIR)/udhcpc.script | _basedep_clean
	$(DOEXE) -- '$(<)' '$(OVERLAY_O:/=)/$(UDHCPC_SCRIPT_FILE)'
	touch '$(@)'

VIRTUAL_OVERLAY_NAMES += dropbear
$(O)/.stamp_overlay_dropbear: $(FILESDIR)/start-dropbear | _basedep_clean
	$(DOEXE) -- '$(<)' '$(OVERLAY_O:/=)/usr/sbin/start-dropbear'
	touch '$(@)'
