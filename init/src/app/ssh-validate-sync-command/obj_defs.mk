ODEF_APP_SSH_VALIDATE_SYNC_COMMAND_ANY     +=
ODEF_APP_SSH_VALIDATE_SYNC_COMMAND_OBJECTS +=
ODEF_APP_SSH_VALIDATE_SYNC_COMMAND_HEADERS +=

$(eval $(call C_OBJ_DEFS,app/ssh-validate-sync-command))

PROG_SSH_VALIDATE_SYNC_COMMAND_SOLIB          =
PROG_SSH_VALIDATE_SYNC_COMMAND_MAIN_SRC       = $(call f_get_objectsv,app/ssh-validate-sync-command/main)
#PROG_SSH_VALIDATE_SYNC_COMMAND_SRC_DEP       += $(call get_c_objects,APP_SSH_VALIDATE_SYNC_COMMAND)
#PROG_SSH_VALIDATE_SYNC_COMMAND_SRC_DEP       += $(call get_c_headers,APP_SSH_VALIDATE_SYNC_COMMAND)
##PROG_SSH_VALIDATE_SYNC_COMMAND_NOLIB_SRC_DEP += $(call f_get_headersv,common/mac)


BINDIR_PROGS += ssh-validate-sync-command
