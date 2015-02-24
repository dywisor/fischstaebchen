ODEF_APP_UNTAR_ANY     += untar
ODEF_APP_UNTAR_OBJECTS +=
ODEF_APP_UNTAR_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/untar))

PROG_UNTAR_SOLIB          =
PROG_UNTAR_MAIN_SRC       = $(call f_get_objectsv,app/untar/main)
PROG_UNTAR_SRC_DEP       += $(call get_c_objects,APP_UNTAR)
PROG_UNTAR_SRC_DEP       += $(call get_c_headers,APP_UNTAR)
PROG_UNTAR_SRC_DEP       +=
PROG_UNTAR_NOLIB_SRC_DEP += $(call f_get_headersv,common/mac)

BINDIR_PROGS += untar
