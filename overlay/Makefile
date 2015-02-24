S := $(CURDIR)
O := $(CURDIR)/build

DEFAULT_OVERLAYS  =
DEFAULT_OVERLAYS += base
DEFAULT_OVERLAYS += misc phaseout
DEFAULT_OVERLAYS += separate_usr squashed_rootfs
DEFAULT_OVERLAYS += union_mount_base union_mount
DEFAULT_OVERLAYS += stagedive
DEFAULT_OVERLAYS += stagedive_bootstrap_arch
DEFAULT_OVERLAYS += stagedive_bootstrap_gentoo
DEFAULT_OVERLAYS += dotfiles
DEFAULT_OVERLAYS += newroot_hooks

PHONY += all
all: overlay | _basedep_clean

__PRJROOT = $(abspath $(S)/..)

VIRTUAL_OVERLAY_NAMES =

include $(__PRJROOT)/mk/basedep.mk
include $(__PRJROOT)/mk/vars.mk
include $(__PRJROOT)/mk/install.mk
include $(__PRJROOT)/mk/util.mk
include $(__PRJROOT)/mk/build_scripts.mk

include $(S)/mk/vars.mk
include $(S)/mk/overlay_def.mk
include $(S)/mk/voverlay.mk

OVERLAY_NAMES := $(filter-out meta META,\
	$(foreach d,$(wildcard $(OVERLAY_SRCDIR)/*),$(notdir $(d))))
$(foreach name,$(OVERLAY_NAMES),$(eval $(call OVERLAY_DEF,$(name))))

ifneq ($(origin OVERLAYS),command line)
_ALL_OVERLAYS = $(DEFAULT_OVERLAYS)
else
_ALL_OVERLAYS = $(OVERLAYS)
endif
_ALL_OVERLAYS += $(VIRTUAL_OVERLAY_NAMES) $(EXTRA_OVERLAYS)

include $(S)/mk/vars_late.mk
include $(S)/mk/genfunc.mk
include $(S)/mk/rules_late.mk

PHONY += clean
clean:
	test -n '$(O)' && test '$(O)' != '$(S)'
	test ! -d '$(O)' || rm -r -- $(O)

PHONY += _install_sanity_check
_install_sanity_check:
	test -n '$(DESTDIR:/=)' ## DESTDIR must be set and not '/'

PHONY += install-overlay
install-overlay: _install_sanity_check
	$(DODIR) -- '$(DESTDIR)'
	$(call f_copy_tree,$(OVERLAY_O),$(DESTDIR))

PHONY += install
install: install-overlay

#PHONY += uninstall-overlay
#uninstall-overlay:
#	$(error cannot uninstall)



FORCE:

.PHONY: $(PHONY)

MAKEFLAGS += -rR
.SUFFIXES: