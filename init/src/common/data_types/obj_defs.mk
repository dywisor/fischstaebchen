ODEF_COMMON_DATA_TYPES_ANY      += dynstr
ODEF_COMMON_DATA_TYPES_ANY      += dynarray str_dynarray
ODEF_COMMON_DATA_TYPES_OBJECTS  +=
ODEF_COMMON_DATA_TYPES_HEADERS  +=

$(eval $(call C_OBJ_DEFS,common/data_types,common))
