_OPTIFLAGS  = -O2
ifneq ($(NO_OPTIMIZE),1)
#_OPTIFLAGS += -funsafe-loop-optimizations
#_OPTIFLAGS += -funroll-loops -funswitch-loops
endif

CC           = gcc
TARGET_CC    = $(CROSS_COMPILE)$(CC)
LD           = ld
TARGET_LD    = $(CROSS_COMPILE)$(LD)

CC_OPTS =

# C Makefiles should export these vars:
ifneq ($(X_FORCE_GOLD),)
export TARGET_CC
export TARGET_LD
CC_OPTS += -B$(__PRJROOT)/mk/gold
endif

__CC_NAME   := \
	$(patsubst %-gcc,gcc,$(patsubst %-clang,clang,$(notdir $(TARGET_CC:/=))))


ifeq ($(__CC_NAME),gcc)
_IS_GCC   = 1
_IS_CLANG = 0
else
ifeq ($(__CC_NAME),clang)
_IS_GCC   = 0
_IS_CLANG = 1
else
$(error Could not detect gcc/clang - specify __CC_NAME $(__CC_NAME))
endif
endif


USE_LTO      ?= 1
USE_GRAPHITE ?= 0

ifeq ($(USE_LTO),1)
_OPTIFLAGS += -flto
ifeq ($(_IS_GCC),1)
_OPTIFLAGS += -fuse-linker-plugin
endif
endif


ifeq ($(USE_GRAPHITE),1)
_OPTIFLAGS += \
	-floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block
endif

# default -W... flags for CFLAGS
_WARNFLAGS  =

ifeq ($(NO_WERROR),)
_WARNFLAGS += -Werror
_WARNFLAGS += -Wno-unused-parameter
_WARNFLAGS += -Wno-unknown-pragmas
endif

_WARNFLAGS += -Wall -Wextra
_WARNFLAGS += -Wwrite-strings -Wdeclaration-after-statement
_WARNFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
_WARNFLAGS += -pedantic
ifeq ($(_IS_GCC),1)
_WARNFLAGS += -Wtrampolines
_WARNFLAGS += -Wunsafe-loop-optimizations
endif
_WARNFLAGS += -Wformat -Wno-format-extra-args -Wformat-security
_WARNFLAGS += -Wformat-nonliteral -Wformat=2




PKG_CONFIG  ?= pkg-config
# note that _OPTIFLAGS takes precedence over the "-O2" in CFLAGS_BASE
CFLAGS_BASE ?= -O2 -pipe
CFLAGS_BASE += $(EXTRA_CFLAGS) $(CFLAGS_EXTRA)
CFLAGS      ?= $(_WARNFLAGS) $(CFLAGS_BASE)
CPPFLAGS    ?=
LDFLAGS     ?= -Wl,-O1 -Wl,--as-needed




CC_OPTS  += -fPIC
CC_OPTS  += -std=gnu99
# _GNU_SOURCE should be set
CC_OPTS += -D_GNU_SOURCE
ifeq ($(STATIC),1)
CC_OPTS += -static
endif

ifeq ($(LOCALCONFIG),)
else
ifeq ($(LOCALCONFIG),y)
CC_OPTS += -DLOCALCONFIG=$(O)/localconfig.h
else
CC_OPTS += -DLOCALCONFIG=$(LOCALCONFIG)
endif
endif

CPPFLAGS += $(EXTRA_CPPFLAGS) $(CPPFLAGS_EXTRA)

_C_COMPILE_FLAGS          = $(CC_OPTS) $(CPPFLAGS) $(CFLAGS) $(_OPTIFLAGS)
_C_COMPILE_FLAGS_NOSTATIC = $(filter-out -static,$(_C_COMPILE_FLAGS))

COMPILE_C        = $(TARGET_CC) $(_C_COMPILE_FLAGS) -c
COMPILE_C_SHARED = $(TARGET_CC) -shared $(_C_COMPILE_FLAGS_NOSTATIC)
LINK_O           = $(TARGET_CC) $(_C_COMPILE_FLAGS) $(LDFLAGS)
LINK_O_SHARED    = $(TARGET_CC) $(_C_COMPILE_FLAGS_NOSTATIC)


ifeq ($(STRIP),1)
TARGET_STRIP_IF_REQUESTED = $(CROSS_COMPILE)strip
else
TARGET_STRIP_IF_REQUESTED = true no-strip
endif
