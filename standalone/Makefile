X_CPIO     ?= cpio
X_FAKEROOT ?= fakeroot

S := $(CURDIR)
O := $(CURDIR)/build
FILESDIR := $(S)/files

X_MKCPIO = $(S)/bin/mkcpio

INITRAMFS := $(O)/rootfs

INITRAMFS_COMPONENTS =

PHONY =

PHONY += default
default: initramfs

# use FEATURES= / OVERLAYS=
INITRAMFS_OVERLAYS =

NONOVERLAY_FEATURES += fsck mkfs

WITH_BROKEN ?= 0

__PRJROOT = $(abspath $(S)/..)

include $(__PRJROOT)/mk/vars.mk
include $(__PRJROOT)/mk/install.mk
include $(__PRJROOT)/mk/util.mk
include $(__PRJROOT)/mk/build_scripts.mk

include $(S)/mk/_init_defs.mk
include $(S)/mk/_build_defs.mk
include $(S)/mk/_init_tool_def.mk

include $(S)/mk/overlay.mk
include $(S)/mk/busybox.mk
include $(S)/mk/init.mk
include $(S)/mk/init_tools.mk
include $(S)/mk/aufs-overlayfs.mk
include $(S)/mk/pv.mk
include $(S)/mk/e2fsprogs.mk

PHONY += clean
clean:
	test -n '$(O)' && test '$(O)' != '$(S)'
	rm -rf -- $(O)

PHONY += build-components
build-components: $(INITRAMFS_COMPONENTS)

PHONY += install-components
install-components: build-components $(addprefix install-,$(INITRAMFS_COMPONENTS))

PHONY += build
build: $(INITRAMFS) install-components

$(INITRAMFS):
	mkdir -p $(@D)

initramfs: $(O)/boot.cpio $(addprefix $(O)/boot.cpio.,xz gz)

$(O)/boot.cpio: $(INITRAMFS) FORCE | build $(X_MKCPIO)
	mkdir -p -- $(@D)
	cd '$(<)' && \
		WITH_BROKEN="$(WITH_BROKEN)" \
		X_CPIO="$(X_CPIO)" \
		OVERLAY_O_FAKEROOTD='$(OVERLAY_O_FAKEROOTD)' \
		$(X_FAKEROOT) $(X_MKCPIO) "$@.new"

	mv -f -- $@.new $@

$(O)/boot.cpio.xz: %.xz: %
	mkdir -p -- $(@D)
	xz -c --check=crc32 < $< > $@.new
	mv -f -- $@.new $@

$(O)/boot.cpio.gz: %.gz: %
	mkdir -p -- $(@D)
	gzip -c < $<  > $@.new
	mv -f -- $@.new $@

FORCE:

.PHONY: $(PHONY)
