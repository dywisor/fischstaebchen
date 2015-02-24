ODEF_COMMON_BASELAYOUT_ANY      += basemounts busybox mdev baselayout
ODEF_COMMON_BASELAYOUT_OBJECTS  +=
ODEF_COMMON_BASELAYOUT_HEADERS  += config

$(eval $(call C_OBJ_DEFS,common/baselayout,common))
