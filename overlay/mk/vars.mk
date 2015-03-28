FILESDIR       := $(S)/misc/files
OVERLAY_SRCDIR := $(abspath $(S)/src)
OVERLAY_O      := $(abspath $(O)/overlay)

ifeq ($(OVERLAY_XSHELL),)
OVERLAY_XSHELL := $(shell \
	$(X_METASH) $(METASH_VDEF_OPTS) --query XSHELL 2>/dev/null)
ifeq ($(OVERLAY_XSHELL),)
override OVERLAY_XSHELL = /bin/sh
endif
endif


OVERLAY_INITDIR_REL         = /init.d
OVERLAY_METASCRIPTDIR_REL   = $(OVERLAY_INITDIR_REL:/=)/metascript
OVERLAY_HOOKDIR_REL         = $(OVERLAY_INITDIR_REL:/=)/hooks
OVERLAY_FUNCTIONSFILE_REL   = $(OVERLAY_INITDIR_REL:/=)/functions.sh
OVERLAY_ENVFILE_REL         = $(OVERLAY_INITDIR_REL:/=)/env.sh

OVERLAY_O_TMPROOT           = $(O)/tmp/overlay

OVERLAY_O_INITDIR           = $(OVERLAY_O:/=)$(OVERLAY_INITDIR_REL)
OVERLAY_O_METASCRIPTDIR     = $(OVERLAY_O:/=)$(OVERLAY_METASCRIPTDIR_REL)
OVERLAY_O_HOOKDIR           = $(OVERLAY_O:/=)$(OVERLAY_HOOKDIR_REL)
OVERLAY_O_FUNCTIONS_FILE    = $(OVERLAY_O:/=)$(OVERLAY_FUNCTIONSFILE_REL)
OVERLAY_O_FUNCTIONS_DIR     = $(O)/overlay-functions.d
OVERLAY_O_ENVFILE_FILE      = $(OVERLAY_O:/=)$(OVERLAY_ENVFILE_REL)
OVERLAY_O_ENVFILES_DIR      = $(O)/overlay-env.d
OVERLAY_O_FAKEROOTD         = $(O)/fakeroot-setup.d
OVERLAY_O_WANTSED           = $(O)/sed_edit.d/overlay

# _f_get_sed_edit ( meta_symbol, repl, expr_delim )
_f_get_sed_edit = s$(3)$(1)$(3)$(2)$(3)g

# f_get_sed_edit ( meta_var, repl )
f_get_sed_edit  = $(call _f_get_sed_edit,@@$(1)@@,$(2),=)

_OVERLAY_SED_EDIT_OPTS  =
_OVERLAY_SED_EDIT_EXPR  =
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,INITDIR,$(OVERLAY_INITDIR_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,HOOKDIR,$(OVERLAY_HOOKDIR_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,FUNCTIONS,$(OVERLAY_FUNCTIONSFILE_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,ENVFILE,$(OVERLAY_ENVFILE_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,METASCRIPTDIR,$(OVERLAY_METASCRIPTDIR_REL))

ifeq ($(SHELLCHECK),)
# drop shellcheck directives ("# shellcheck disable=...")
#  need to escape #, +, = and ? chars..
_OVERLAY_SED_EDIT_EXPR += \
	/^\s*[\#]\s*shellcheck\s\+disable\=\(SC[0-9]\+\([,]SC[0-9]\+\)*\)\?\s*$$/d

endif

ifneq ($(OVERLAY_XSHELL),)
_OVERLAY_SED_EDIT_OPTS += -e '$(call f_get_sed_edit,XSHELL,$(OVERLAY_XSHELL))'
endif

OVERLAY_SED_EDIT_EXPR := \
	$(foreach e,$(_OVERLAY_SED_EDIT_EXPR),-e '$(e)') \
	$(_OVERLAY_SED_EDIT_OPTS)



ifeq ($(OVERLAY_SED_EDIT_EXPR),)
OVERLAY_SED_EDIT = true
else
OVERLAY_SED_EDIT = $(SED) $(OVERLAY_SED_EDIT_EXPR) -i --
endif

# vars handled by sed-edit
METASH_OPTS += $(foreach x,INITDIR HOOKDIR FUNCTIONS ENVFILE METASCRIPTDIR,-V $(x)=@@$(x)@@)

ifneq ($(OVERLAY_XSHELL),)
METASH_OPTS += $(foreach x,XSHELL,-V $(x)=@@$(x)@@)
endif
