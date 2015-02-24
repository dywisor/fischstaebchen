# _INIT_TOOL_DEF ( prog_name )
define _INIT_TOOL_DEF

$(eval INITRAMFS_COMPONENTS += $(1))
$(eval NONOVERLAY_FEATURES  += $(1))

PHONY += $(1)
$(1): $(O)/init/bin/$(1)

PHONY += install-$(1)
install-$(1): $(1)
	$(DOEXE) -- $(O)/init/bin/$(1) $(INITRAMFS)/bin/$(1)
	$$(call ITOOL_INSTALL_$(1),$(1))

endef

INIT_TOOL_DEF = $(_INIT_TOOL_DEF)
