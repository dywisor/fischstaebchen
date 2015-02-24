S      := $(CURDIR)
O      := $(CURDIR)/build

__PRJROOT := $(abspath $(S)/..)

OBIN   := $(O)/bin
OLIB   := $(O)/lib
SRCDIR := $(S)/src

PHONY  =

PHONY += all
all: progs

include $(__PRJROOT)/mk/vars.mk
include $(__PRJROOT)/mk/install.mk
include $(__PRJROOT)/mk/util.mk
include $(__PRJROOT)/mk/build_scripts.mk

include $(__PRJROOT)/mk/c_vars.mk
include $(__PRJROOT)/mk/c_rules.mk
include $(__PRJROOT)/mk/c_librules.mk
include $(__PRJROOT)/mk/c_progrules.mk

include $(S)/mk/obj_defs/base.mk
include $(S)/mk/obj_defs/libs.mk
include $(S)/mk/obj_defs/progs.mk

ifeq ($(PROG_NAMES),)
$(error PROG_NAMES is not set)
endif

PHONY += progs
progs: $(PROG_NAMES)

##PHONY += libs
##libs: $(LIB_NAMES)

PHONY += clean
clean:
	test -n '$(O)' && test '$(O)' != '$(S)'
	test ! -d '$(O)' || rm -r -- $(O)

FORCE:

.PHONY: $(PHONY)

MAKEFLAGS += -rR
.SUFFIXES:
