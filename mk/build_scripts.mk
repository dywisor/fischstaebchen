ifeq ($(__PRJROOT),)
$(error __PRJROOT is not set)
endif

BUILDSCRIPTS_DIR := $(__PRJROOT)/scripts

# shellcheck
X_SHELLCHECK = $(BUILDSCRIPTS_DIR)/run-shellcheck

# metash
X_METASH = $(BUILDSCRIPTS_DIR)/metash

METASH_VDEF_OPTS = -F '$(__PRJROOT)/misc/metash_vdef'
ifneq ($(METASH_DEFAULTS_FILE),)
METASH_VDEF_OPTS += -F '$(METASH_DEFAULTS_FILE)'
endif

METASH_VDEF_OPTS += $(foreach f,$(METASH_DEFAULTS_FILES),-F '$(f)')

RUN_METASH = $(X_METASH) $(METASH_VDEF_OPTS) $(METASH_OPTS)

# f_run_metash_convert_dir ( srcdir, outdir, ["--stdin"], [extra_opts] )
define f_run_metash_convert_dir
	$(SHELL) $(BUILDSCRIPTS_DIR)/run-metash $(3) \
		'$(strip $(1))' '$(strip $(2))' -- $(RUN_METASH) $(4)
endef

# f_run_metash_convert_dir_ifexist ( srcdir, outdir, ["--stdin"], [extra_opts] )
define f_run_metash_convert_dir_ifexist
	test ! -e '$(strip $(1))' || \
		$(call f_run_metash_convert_dir,$(1),$(2),$(3),$(4))
endef
