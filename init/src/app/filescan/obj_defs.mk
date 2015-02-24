ODEF_APP_FILESCAN_ANY     +=
ODEF_APP_FILESCAN_OBJECTS +=
ODEF_APP_FILESCAN_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/filescan))

PROG_FILESCAN_SOLIB          =
PROG_FILESCAN_MAIN_SRC       = $(call f_get_objectsv,app/filescan/main)
PROG_FILESCAN_SRC_DEP       += $(call get_c_objects,APP_FILESCAN)
PROG_FILESCAN_SRC_DEP       += $(call get_c_headers,APP_FILESCAN)
PROG_FILESCAN_SRC_DEP       +=
#PROG_FILESCAN_NOLIB_SRC_DEP += $(call f_get_headersv,common/mac)

BINDIR_PROGS += filescan
