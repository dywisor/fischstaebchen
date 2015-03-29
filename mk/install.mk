DESTDIR     =
PREFIX      = /usr/local
EXEC_PREFIX = $(PREFIX)
BINDIR      = $(EXEC_PREFIX:/=)/bin
SBINDIR     = $(EXEC_PREFIX:/=)/sbin
LIBDIR_NAME = lib
LIBDIR      = $(EXEC_PREFIX:/=)/$(LIBDIR_NAME)

EXEMODE ?= 0755
INSMODE ?= 0644
DIRMODE ?= 0755

DODIR    = $(INSTALL) -d -m $(DIRMODE)
DOEXE    = $(INSTALL) -D -m $(EXEMODE)
DOINS    = $(INSTALL) -D -m $(INSMODE)
DOSYM    = $(LN) -f -s

ifeq ($(CP_RECURSIVE),)
ifeq ($(shell $(CP) --help 2>&1 | grep -E -- '\s--no-preserve='),)
CP_RECURSIVE = $(CP) -dpRf
else
CP_RECURSIVE = $(CP) -dpRf --no-preserve=ownership
endif
endif

# f_copy_tree ( src, dst )
#
ifeq ($(X_RSYNC),)
define f_copy_tree
	{ \
		$(DODIR) -- '$(2)' && \
		$(CP_RECURSIVE) -- '$(1)/.' '$(2)/.'; \
	}
endef
else
define f_copy_tree
	{ \
		$(DODIR) -- '$(2)' && \
		$(X_RSYNC) -rlptD -- '$(1)/' '$(2)/'; \
	}
endef
endif

# f_copy_tree_ifexist ( src, dst )
#  (call() not necessary here..)
#
define f_copy_tree_ifexist
	{ if test -d '$(1)'; then $(call f_copy_tree,$(1),$(2)); fi; }
endef
