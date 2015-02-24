X_INIT_CONFFILE  = $(O)/init/_localconfig.h

X_INIT_MAKEOPTS  = -C '$(__PRJROOT)/init' O=$(O)/init
X_INIT_MAKEOPTS += STATIC=1 STRIP=1
X_INIT_MAKEOPTS += CROSS_COMPILE=$(CROSS_COMPILE)
X_INIT_MAKEOPTS += ARCH=$(ARCH) TARGET_ARCH=$(TARGET_ARCH) HOST_ARCH=$(HOST_ARCH)
X_INIT_MAKEOPTS += EXTRA_CPPFLAGS="-DLOCALCONFIG=$(X_INIT_CONFFILE)"


$(X_INIT_CONFFILE): $(FILESDIR)/localconfig.h
	mkdir -p -- $(@D)

	cp -- $< $@.tmp
ifneq ($(filter mkfs,$(FEATURES)),)
	$(DOSED_TMPFILE) \
		s,\(^\[\#\]define\\sZRAM_DISK_ENABLE_\(EXT4\|EXT2_EXTERN\)\\s*\)0,\\11,
endif
	mv -f -- $@.tmp $@

# static: compile with uclibc
$(O)/init/bin/%: $(X_INIT_CONFFILE)
	$(MAKE) $(X_INIT_MAKEOPTS) $(@F)
