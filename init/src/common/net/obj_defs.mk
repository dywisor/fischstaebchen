ODEF_COMMON_NET_ANY      += packet_sender
ODEF_COMMON_NET_ANY      += magic_packet_sender
ODEF_COMMON_NET_OBJECTS  +=
ODEF_COMMON_NET_HEADERS  += data_types

include $(SRCDIR)/common/net/data_types/obj_defs.mk

$(eval $(call C_OBJ_DEFS,common/net,common))
