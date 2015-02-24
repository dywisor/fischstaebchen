ODEF_APP_CREATE_USER_TMPDIRS_ANY     += create-user-tmpdirs
ODEF_APP_CREATE_USER_TMPDIRS_OBJECTS +=
ODEF_APP_CREATE_USER_TMPDIRS_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/create-user-tmpdirs))

PROG_CREATE_USER_TMPDIRS_SOLIB          =
PROG_CREATE_USER_TMPDIRS_MAIN_SRC       = $(call f_get_objectsv,app/create-user-tmpdirs/main)
PROG_CREATE_USER_TMPDIRS_SRC_DEP       += $(call get_c_objects,APP_CREATE_USER_TMPDIRS)
PROG_CREATE_USER_TMPDIRS_SRC_DEP       += $(call get_c_headers,APP_CREATE_USER_TMPDIRS)
PROG_CREATE_USER_TMPDIRS_NOLIB_SRC_DEP += $(call f_get_headersv,common/mac)


SBINDIR_PROGS += create-user-tmpdirs
