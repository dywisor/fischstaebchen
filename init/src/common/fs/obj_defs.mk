ODEF_COMMON_FS_ANY      += baseops devfs fileio fspath loopdev findfs umount
ODEF_COMMON_FS_ANY      += mount_opts mount_config mount stat touch
ODEF_COMMON_FS_ANY      += proc_misc
#ODEF_COMMON_FS_ANY      += union_mount_base overlayfs_layer overlayfs
ODEF_COMMON_FS_OBJECTS  +=
ODEF_COMMON_FS_HEADERS  += constants

$(eval $(call C_OBJ_DEFS,common/fs,common))
