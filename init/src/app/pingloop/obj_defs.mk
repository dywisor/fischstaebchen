ODEF_APP_PINGLOOP_ANY     += pingloop
ODEF_APP_PINGLOOP_OBJECTS +=
ODEF_APP_PINGLOOP_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/pingloop))

PROG_PINGLOOP_ANY_NAMES       += common/misc/run_command
PROG_PINGLOOP_ANY_NAMES       += common/data_types/dynarray
PROG_PINGLOOP_HEADERS_NAMES   += common/mac
PROG_PINGLOOP_ANY_NAMES       += common/message
PROG_PINGLOOP_ANY_NAMES       += common/strutil/join
PROG_PINGLOOP_ANY_NAMES       += common/data_types/dynstr
PROG_PINGLOOP_ANY_NAMES       += common/strutil/convert
PROG_PINGLOOP_ANY_NAMES       += common/strutil/format

PROG_PINGLOOP_MAIN_SRC       = $(call f_get_objectsv,app/pingloop/main)
PROG_PINGLOOP_SRC_DEP       += $(call get_c_objects,APP_PINGLOOP)
PROG_PINGLOOP_SRC_DEP       += $(call get_c_headers,APP_PINGLOOP)
PROG_PINGLOOP_NOLIB_SRC_DEP += $(call get_c_objects,PROG_PINGLOOP)
PROG_PINGLOOP_NOLIB_SRC_DEP += $(call get_c_headers,PROG_PINGLOOP)

BINDIR_PROGS += pingloop
