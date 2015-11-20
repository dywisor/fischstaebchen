ODEF_COMMON_ANY      += message
ODEF_COMMON_ANY      += decision_bool
ODEF_COMMON_OBJECTS  +=
ODEF_COMMON_HEADERS  += config message_defs mac
ODEF_COMMON_HEADERS  += data_types
ODEF_COMMON_HEADERS  += fs misc strutil

include $(SRCDIR)/common/data_types/obj_defs.mk
include $(SRCDIR)/common/fs/obj_defs.mk
include $(SRCDIR)/common/misc/obj_defs.mk
include $(SRCDIR)/common/net/obj_defs.mk
include $(SRCDIR)/common/strutil/obj_defs.mk
include $(SRCDIR)/common/baselayout/obj_defs.mk


$(eval $(call C_OBJ_DEFS,common))
