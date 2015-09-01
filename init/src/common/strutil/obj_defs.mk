ODEF_COMMON_STRUTIL_ANY      += compare convert join split argsplit misc
ODEF_COMMON_STRUTIL_ANY      += format
ODEF_COMMON_STRUTIL_OBJECTS  +=
ODEF_COMMON_STRUTIL_HEADERS  +=

$(eval $(call C_OBJ_DEFS,common/strutil,common))
