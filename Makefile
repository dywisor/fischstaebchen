S := $(CURDIR)

__PRJROOT := $(abspath $(S)/..)

_SUBPRJ_LIST =
_SUBPRJ_LIST += init
_SUBPRJ_LIST += overlay
_SUBPRJ_LIST += shellfunc
_SUBPRJ_LIST += standalone

_SUBPRJ_CLEAN_TARGETS := $(addprefix clean-,$(_SUBPRJ_LIST))

PHONY += all
all:
	false

PHONY += clean
clean: $(_SUBPRJ_CLEAN_TARGETS)

PHONY += $(_SUBPRJ_CLEAN_TARGETS)
$(_SUBPRJ_CLEAN_TARGETS): clean-%: $(S)/%
	$(MAKE) -C "$(<)" clean

FORCE:

.PHONY: $(PHONY)

MAKEFLAGS += -rR
.SUFFIXES:
