ODEF_COMMON_NET_DATA_TYPES_ANY      += mac_addr
ODEF_COMMON_NET_DATA_TYPES_ANY      += magic_packet
ODEF_COMMON_NET_DATA_TYPES_OBJECTS  +=
ODEF_COMMON_NET_DATA_TYPES_HEADERS  +=

$(eval $(call C_OBJ_DEFS,common/net/data_types,common/net))
