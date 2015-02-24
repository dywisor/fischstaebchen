FILESDIR       := $(S)/misc/files
OVERLAY_SRCDIR := $(abspath $(S)/src)
OVERLAY_O      := $(abspath $(O)/overlay)

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


_OVERLAY_SED_EDIT_EXPR  =
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,INITDIR,$(OVERLAY_INITDIR_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,HOOKDIR,$(OVERLAY_HOOKDIR_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,FUNCTIONS,$(OVERLAY_FUNCTIONSFILE_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,ENVFILE,$(OVERLAY_ENVFILE_REL))
_OVERLAY_SED_EDIT_EXPR += $(call f_get_sed_edit,METASCRIPTDIR,$(OVERLAY_METASCRIPTDIR_REL))

OVERLAY_SED_EDIT_EXPR  := $(foreach e,$(_OVERLAY_SED_EDIT_EXPR),-e '$(e)')

ifeq ($(OVERLAY_SED_EDIT_EXPR),)
OVERLAY_SED_EDIT = true
else
OVERLAY_SED_EDIT = $(SED) $(OVERLAY_SED_EDIT_EXPR) -i --
endif

X_METASH     = $(BUILDSCRIPTS_DIR)/metash
METASH_OPTS  = -F $(S)/misc/metash_vdef
ifneq ($(OVERLAY_DEFAULTS_FILE),)
METASH_OPTS += -F $(OVERLAY_DEFAULTS_FILE)
endif

# vars handled by sed-edit
METASH_OPTS += $(foreach x,INITDIR HOOKDIR FUNCTIONS ENVFILE METASCRIPTDIR,-V $(x)=@@$(x)@@)

RUN_METASH  = $(X_METASH) $(METASH_OPTS)
