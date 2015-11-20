ODEF_APP_READ_PROC_ANY     += _base
ODEF_APP_READ_PROC_OBJECTS +=
ODEF_APP_READ_PROC_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/read-proc))

PROG__PROC_READ_HEADERS_NAMES   += common/mac
PROG__PROC_READ_ANY_NAMES       += common/data_types/dynarray
PROG__PROC_READ_ANY_NAMES       += common/message
PROG__PROC_READ_ANY_NAMES       += common/strutil/join
PROG__PROC_READ_ANY_NAMES       += common/data_types/dynstr
PROG__PROC_READ_ANY_NAMES       += common/fs/process_info
PROG__PROC_READ_HEADERS_NAMES   += common/config
PROG__PROC_READ_ANY_NAMES       += common/data_types/str_dynarray
PROG__PROC_READ_ANY_NAMES       += common/strutil/compare
PROG__PROC_READ_ANY_NAMES       += common/strutil/convert
PROG__PROC_READ_ANY_NAMES       += common/strutil/format
PROG__PROC_READ_ANY_NAMES       += common/strutil/lazy_basename


## read-proc-cmdline:
PROG_READ_PROC_CMDLINE_OBJECTS_NAMES += $(PROG__PROC_READ_OBJECTS_NAMES)
PROG_READ_PROC_CMDLINE_OBJECTS_NAMES += app/read-proc/read-proc-cmdline
PROG_READ_PROC_CMDLINE_HEADERS_NAMES += $(PROG__PROC_READ_HEADERS_NAMES)
PROG_READ_PROC_CMDLINE_ANY_NAMES     += $(PROG__PROC_READ_ANY_NAMES)

#PROG_READ_PROC_CMDLINE_SOLIB          =
PROG_READ_PROC_CMDLINE_MAIN_SRC       = $(call f_get_objectsv,app/read-proc/read-proc-cmdline)
PROG_READ_PROC_CMDLINE_SRC_DEP       += $(call get_c_objects,APP_READ_PROC)
PROG_READ_PROC_CMDLINE_SRC_DEP       += $(call get_c_headers,APP_READ_PROC)
PROG_READ_PROC_CMDLINE_SRC_DEP       +=
PROG_READ_PROC_CMDLINE_NOLIB_SRC_DEP += $(call get_c_objects,PROG_READ_PROC_CMDLINE)
PROG_READ_PROC_CMDLINE_NOLIB_SRC_DEP += $(call get_c_headers,PROG_READ_PROC_CMDLINE)

BINDIR_PROGS += read-proc-cmdline


## read-proc-environ:
PROG_READ_PROC_ENVIRON_OBJECTS_NAMES += $(PROG__PROC_READ_OBJECTS_NAMES)
PROG_READ_PROC_ENVIRON_OBJECTS_NAMES += app/read-proc/read-proc-environ
PROG_READ_PROC_ENVIRON_HEADERS_NAMES += $(PROG__PROC_READ_HEADERS_NAMES)
PROG_READ_PROC_ENVIRON_ANY_NAMES     += $(PROG__PROC_READ_ANY_NAMES)

#PROG_READ_PROC_ENVIRON_SOLIB          =
PROG_READ_PROC_ENVIRON_MAIN_SRC       = $(call f_get_objectsv,app/read-proc/read-proc-environ)
PROG_READ_PROC_ENVIRON_SRC_DEP       += $(call get_c_objects,APP_READ_PROC)
PROG_READ_PROC_ENVIRON_SRC_DEP       += $(call get_c_headers,APP_READ_PROC)
PROG_READ_PROC_ENVIRON_SRC_DEP       +=
PROG_READ_PROC_ENVIRON_NOLIB_SRC_DEP += $(call get_c_objects,PROG_READ_PROC_ENVIRON)
PROG_READ_PROC_ENVIRON_NOLIB_SRC_DEP += $(call get_c_headers,PROG_READ_PROC_ENVIRON)

BINDIR_PROGS += read-proc-environ
