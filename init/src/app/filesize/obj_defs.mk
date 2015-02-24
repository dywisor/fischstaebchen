ODEF_APP_FILESIZE_ANY     +=
ODEF_APP_FILESIZE_OBJECTS +=
ODEF_APP_FILESIZE_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/filesize))

PROG_FILESIZE_SOLIB          =
PROG_FILESIZE_MAIN_SRC       = $(call f_get_objectsv,app/filesize/main)
PROG_FILESIZE_SRC_DEP       += $(call get_c_objects,APP_FILESIZE)
PROG_FILESIZE_SRC_DEP       += $(call get_c_headers,APP_FILESIZE)
PROG_FILESIZE_SRC_DEP       +=
#PROG_FILESIZE_NOLIB_SRC_DEP += $(call f_get_headersv,common/mac)

BINDIR_PROGS += filesize
