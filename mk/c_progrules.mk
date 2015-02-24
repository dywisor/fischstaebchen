# _C_PROG_VARS_DEF ( make_name, prog_name )
#
define _C_PROG_VARS_DEF
$(eval PROG_$(1)_NAME          ?= $(2))
$(eval PROG_$(1)_BUILD          = $(OBIN)/$(PROG_$(1)_NAME))
$(eval PROG_$(1)_MAIN_SRC      ?= )
$(eval PROG_$(1)_SRC_DEP       ?= )
$(eval PROG_$(1)_NOLIB_SRC_DEP ?= )
$(eval PROG_$(1)_SOLIB         ?= LIBFISCHSTAEBCHEN_ANY)
$(eval PROG_$(1)_SOLIB_INCLUDE ?= $($(PROG_$(1)_SOLIB)_INCLUDE))
$(eval PROG_$(1)_SOLIB_DEP     ?= $($(PROG_$(1)_SOLIB)_DEP))
endef

# _C_PROG_DEF_<variant> ( make_name, prog_name~ )
#

# shared libc (no shared private lib)
define _C_PROG_DEF_NONSHARED
$(PROG_$(1)_BUILD): \
	$(PROG_$(1)_MAIN_SRC) $(PROG_$(1)_SRC_DEP) $(PROG_$(1)_NOLIB_SRC_DEP)

	$(MKDIRP) -- $$(@D)
	$(LINK_O) $$(filter-out %.h,$$^) -o $$@
	$(TARGET_STRIP_IF_REQUESTED) $$@
endef

# shared lib
define _C_PROG_DEF_SHARED_LIB
ifeq ($(PROG_$(1)_SOLIB),)
$(call _C_PROG_DEF_NONSHARED,$(1),$(2))

else
$(PROG_$(1)_BUILD): \
	$(PROG_$(1)_MAIN_SRC) $(PROG_$(1)_SRC_DEP) $(PROG_$(1)_SOLIB_DEP)

	$(MKDIRP) -- $$(@D)
	$(LINK_O_SHARED) \
		$(PROG_$(1)_MAIN_SRC) \
		-L$(OLIB) -l$(PROG_$(1)_SOLIB_INCLUDE) \
		$$(filter-out %.h,$(PROG_$(1)_SRC_DEP)) \
		-o $$@
	$(TARGET_STRIP_IF_REQUESTED) $$@
endif
endef

# static build, sames as nonshared
#  c_vars takes care of the necessary cflags
define _C_PROG_DEF_STATIC
$(call _C_PROG_DEF_NONSHARED,$(1),$(2))
endef

# _C_PROG_DEF ( make_name, prog_name )
#
ifeq ($(STATIC),1)
_C_PROG_DEF = _C_PROG_DEF_STATIC
else
ifeq ($(NONSHARED),1)
_C_PROG_DEF = _C_PROG_DEF_NONSHARED
else
_C_PROG_DEF = _C_PROG_DEF_SHARED_LIB
endif
endif

define _C_PROG_DEF_META
$(call _C_PROG_VARS_DEF,$(1),$(2))

$(call $(_C_PROG_DEF),$(1),$(PROG_$(1)_NAME))

$(eval PHONY += $(PROG_$(1)_BUILD))
$(PROG_$(1)_NAME): $(PROG_$(1)_BUILD)

$(eval PROG_NAMES += $(PROG_$(1)_NAME))
endef

# _C_PROG_DEF_META_WITH_INSTALL ( make_name, prog_name )
define _C_PROG_DEF_META_WITH_INSTALL
$(call _C_PROG_DEF_META,$(1),$(2))
$(call DEF_INSTALL_TARGET,$(2))
endef

# _C_PROG_DEF_META_WITH_STD_INSTALL ( exedir, make_name, prog_name )
define _C_PROG_DEF_META_WITH_STD_INSTALL
$(call _C_PROG_DEF_META,$(2),$(3))
$(call DEF_INSTALL_TARGET,$(3))

install-$(3):
	$(DOEXE) -- $(PROG_$(2)_BUILD) $(DESTDIR:/=)$(1:/=)/$(3)
	$$(call PROG_$(2)_INSTALL,$(DESTDIR:/=)$(1:/=)/)

uninstall-$(3):
	$(RMF) -- $(DESTDIR:/=)$(1:/=)/$(3)
	$$(call PROG_$(2)_UNINSTALL,$(DESTDIR:/=)$(1:/=)/)

endef

# C_PROG_DEF ( prog_name )
C_PROG_DEF = $(call _C_PROG_DEF_META,$(call _f_convert_name,$(1)),$(1))

# C_PROG_DEF_WITH_INSTALL ( prog_name )
C_PROG_DEF_WITH_INSTALL = \
	$(call _C_PROG_DEF_META_WITH_INSTALL,$(call _f_convert_name,$(1)),$(1))

# C_PROG_DEF_WITH_BINDIR_INSTALL ( prog_name )
C_PROG_DEF_WITH_BINDIR_INSTALL = \
	$(call _C_PROG_DEF_META_WITH_STD_INSTALL,$(BINDIR),$(call _f_convert_name,$(1)),$(1))

# C_PROG_DEF_WITH_SBINDIR_INSTALL ( prog_name )
C_PROG_DEF_WITH_SBINDIR_INSTALL = \
	$(call _C_PROG_DEF_META_WITH_STD_INSTALL,$(SBINDIR),$(call _f_convert_name,$(1)),$(1))
