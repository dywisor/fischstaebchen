ODEF_ZRAM_ANY      += globals data_types zallocate devfs swap mkfs mount disk
ODEF_ZRAM_ANY      += autoswap tmpfs
ODEF_ZRAM_OBJECTS  +=
ODEF_ZRAM_HEADERS  += config


$(eval $(call C_OBJ_DEFS,zram))
