S := $(CURDIR)
O := $(CURDIR)/build

PHONY += all
all: functions | _basedep_clean

__PRJROOT = $(abspath $(S)/..)

include $(__PRJROOT)/mk/basedep.mk
include $(__PRJROOT)/mk/vars.mk
include $(__PRJROOT)/mk/install.mk
include $(__PRJROOT)/mk/util.mk
include $(__PRJROOT)/mk/build_scripts.mk
include $(__PRJROOT)/mk/shellgen.mk
include $(__PRJROOT)/mk/shfunc_def.mk

include $(S)/mk/funcfiles.mk

FUNCTIONS = base

PHONY += clean
clean:
	test -n '$(O)' && test '$(O)' != '$(S)'
	test ! -d '$(O)' || rm -r -- $(O)


PHONY += shellcheck
shellcheck:
	$(call f_run_shell_syntax_check_recursive,$(FUNCTIONS_O))


PHONY += functions
functions: \
	$(addprefix $(FUNCTIONS_O)/,$(addsuffix .sh,$(FUNCTIONS))) | _basedep_clean


PHONY += install
install:
	$(DODIR) -- '$(DESTDIR)$(SHFUNC_LIBDIR)'
	$(call f_copy_tree,$(FUNCTIONS_O),$(DESTDIR)$(SHFUNC_LIBDIR))



FORCE:

.PHONY: $(PHONY)

MAKEFLAGS += -rR
.SUFFIXES:
