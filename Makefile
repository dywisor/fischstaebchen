S := $(CURDIR)

__PRJROOT := $(abspath $(S))

_SUBPRJ_LIST =
_SUBPRJ_LIST += init
_SUBPRJ_LIST += overlay
_SUBPRJ_LIST += shellfunc
_SUBPRJ_LIST += standalone

_SUBPRJ_CLEAN_TARGETS := $(addprefix clean-,$(_SUBPRJ_LIST))

X_GIT = git


PHONY += all
all:
	false

PHONY += clean
clean: $(_SUBPRJ_CLEAN_TARGETS)

PHONY += $(_SUBPRJ_CLEAN_TARGETS)
$(_SUBPRJ_CLEAN_TARGETS): clean-%: $(S)/%
	$(MAKE) -C "$(<)" clean

PHONY += setver
ifeq ($(VER),)
setver:
	$(error VER is not set)
else

setver:
	printf '%s\n' '$(VER)' > '$(__PRJROOT)/VERSION'
	$(X_GIT) -C '$(__PRJROOT)' add    './VERSION' && \
	$(X_GIT) -C '$(__PRJROOT)' commit './VERSION' \
		-m 'fischstaebchen $(VER)' -e
endif


FORCE:

.PHONY: $(PHONY)

MAKEFLAGS += -rR
.SUFFIXES:
