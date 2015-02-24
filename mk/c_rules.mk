ifeq ($(SRCDIR),)
$(error SRCDIR must be set before including c_rules.mk)
endif

ifeq ($(O),)
$(error O must be set before including c_rules.mk)
endif


f_get_objectsv  = $(addprefix $(O)/,$(addsuffix .o,$(1)))
f_get_headersv  = $(addprefix $(SRCDIR)/,$(addsuffix .h,$(1)))


define _C_OBJ_DEFS
$(eval $(1)_ANY_NAMES     += $(addprefix $(2)/,$(ODEF_$(1)_ANY)))
$(eval $(1)_OBJECTS_NAMES += $(addprefix $(2)/,$(ODEF_$(1)_OBJECTS)))
$(eval $(1)_HEADERS_NAMES += $(addprefix $(2)/,$(ODEF_$(1)_HEADERS)))

$(eval $(3)_ANY_NAMES     += $($(1)_ANY_NAMES))
$(eval $(3)_OBJECTS_NAMES += $($(1)_OBJECTS_NAMES))
$(eval $(3)_HEADERS_NAMES += $($(1)_HEADERS_NAMES))
endef

C_OBJ_DEFS = $(call _C_OBJ_DEFS,$(call f_convert_name,$(1)),$(1),$(call f_convert_name,$(2)))

C_RULE_DO_LINK_ONLY  = mkdir -p -- $(@D) && $(LINK_O) $^ -o $@
C_RULE_DO_STRIP_ONLY = $(TARGET_STRIP_IF_REQUESTED) -s $@

define C_RULE_DO_LINK
	$(C_RULE_DO_LINK_ONLY)
	$(C_RULE_DO_STRIP_ONLY)
endef
#C_RULE_DO_STRIP_IF_REQUESTED = $(TARGET_STRIP_IF_REQUESTED) $@

get_c_headers = \
	$(foreach x,$(1),\
		$(call f_get_headersv,$($(x)_ANY_NAMES) $($(x)_HEADERS_NAMES)))

get_c_objects = \
	$(foreach x,$(1),\
		$(call f_get_objectsv,$($(x)_ANY_NAMES) $($(x)_OBJECTS_NAMES)))


$(O)/%.o: $(SRCDIR)/%.c
	mkdir -p -- $(@D)
	$(COMPILE_C) $< -o $@
