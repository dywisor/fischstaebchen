_BASEDEP_CLEAN := $(filter clean clean-*,$(MAKECMDGOALS))

ifneq ($(X_MK_NODEF_REBUILD),1)
ifneq ($(filter rebuild,$(MAKECMDGOALS)),)
_BASEDEP_CLEAN += clean
endif

PHONY += rebuild
rebuild: clean all

endif

_basedep_clean: FORCE $(_BASEDEP_CLEAN)
	@true
