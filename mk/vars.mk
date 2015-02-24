unexport LC_ALL
LC_COLLATE=C
LC_NUMERIC=C
export LC_COLLATE LC_NUMERIC

SHELL ?= sh

HOST_ARCH   := $(shell uname -m)
ARCH        ?= $(HOST_ARCH)
TARGET_ARCH  = $(ARCH)


f_which = $(shell which '$(1)' 2>/dev/null)

SED      = sed
CP       = cp
RM       = rm
LN       = ln
INSTALL  = install
MKDIR    = mkdir
CHMOD    = chmod
CHOWN    = chown
X_RSYNC  = $(call f_which,rsync)

define DEF_INSTALL_TARGET
$(eval PHONY         += install-$(1))
$(eval PHONY         += uninstall-$(1))
$(eval INSTALL_NAMES += $(1))
endef

COMMA := ,
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
