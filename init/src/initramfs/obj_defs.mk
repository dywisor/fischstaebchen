ODEF_INITRAMFS_ANY      += base baselayout globals devfs cmdline initramfs
ODEF_INITRAMFS_ANY      += premount domount ldconfig
ODEF_INITRAMFS_ANY      += run-flags-path run-flags
ODEF_INITRAMFS_ANY      += hacks
ODEF_INITRAMFS_OBJECTS  +=
ODEF_INITRAMFS_HEADERS  += config


$(eval $(call C_OBJ_DEFS,initramfs))
