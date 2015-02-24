ODEF_APP_ZTMPFS_ANY     +=
ODEF_APP_ZTMPFS_OBJECTS +=
ODEF_APP_ZTMPFS_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/ztmpfs))

# automatically generated:
PROG_ZTMPFS_ANY_NAMES       += zram/tmpfs
PROG_ZTMPFS_ANY_NAMES       += zram/disk
PROG_ZTMPFS_ANY_NAMES       += zram/data_types
PROG_ZTMPFS_HEADERS_NAMES   += zram/config
PROG_ZTMPFS_HEADERS_NAMES   += common/config
PROG_ZTMPFS_ANY_NAMES       += zram/globals
PROG_ZTMPFS_ANY_NAMES       += common/strutil/convert
PROG_ZTMPFS_ANY_NAMES       += common/strutil/join
PROG_ZTMPFS_ANY_NAMES       += common/dynstr
PROG_ZTMPFS_ANY_NAMES       += common/dynarray
PROG_ZTMPFS_ANY_NAMES       += common/message
PROG_ZTMPFS_ANY_NAMES       += zram/zallocate
PROG_ZTMPFS_ANY_NAMES       += common/fs/fileio
PROG_ZTMPFS_ANY_NAMES       += common/strutil/split
PROG_ZTMPFS_ANY_NAMES       += zram/mkfs
PROG_ZTMPFS_ANY_NAMES       += zram/devfs
PROG_ZTMPFS_ANY_NAMES       += common/fs/baseops
PROG_ZTMPFS_ANY_NAMES       += common/fs/fspath
PROG_ZTMPFS_HEADERS_NAMES   += common/fs/constants
PROG_ZTMPFS_ANY_NAMES       += common/fs/devfs
PROG_ZTMPFS_ANY_NAMES       += common/fs/stat
PROG_ZTMPFS_ANY_NAMES       += common/fs/proc
PROG_ZTMPFS_ANY_NAMES       += common/misc/run_command
PROG_ZTMPFS_HEADERS_NAMES   += common/mac
PROG_ZTMPFS_ANY_NAMES       += zram/mount
PROG_ZTMPFS_ANY_NAMES       += common/fs/mount
PROG_ZTMPFS_ANY_NAMES       += common/fs/mount_opts
PROG_ZTMPFS_ANY_NAMES       += common/strutil/compare
PROG_ZTMPFS_ANY_NAMES       += common/misc/sysinfo
PROG_ZTMPFS_ANY_NAMES       += common/misc/env
PROG_ZTMPFS_ANY_NAMES       += common/fs/mount_config
PROG_ZTMPFS_ANY_NAMES       += common/fs/findfs
PROG_ZTMPFS_ANY_NAMES       += common/fs/loopdev

PROG_ZTMPFS_MAIN_SRC       = $(call f_get_objectsv,app/ztmpfs/main)
PROG_ZTMPFS_NOLIB_SRC_DEP += $(call get_c_objects,PROG_ZTMPFS)
PROG_ZTMPFS_NOLIB_SRC_DEP += $(call get_c_headers,PROG_ZTMPFS)

SBINDIR_PROGS += ztmpfs

define PROG_ZTMPFS_INSTALL
	$(DOSYM) -- ztmpfs $(1:/=)/zdisk
endef

define PROG_ZTMPFS_UNINSTALL
	$(RMF) -- $(DESTDIR:/=)$(SBINDIR)/zdisk
endef
