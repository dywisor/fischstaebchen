_BDEFS_FLAGS_APPEND = -static
#_BDEFS_FLAGS_APPEND += -Wno-error

C_MAKEENV_STATIC  =
C_MAKEENV_STATIC += CROSS_COMPILE="$(CROSS_COMPILE)"
C_MAKEENV_STATIC += CFLAGS="$(CFLAGS_BASE) $(_BDEFS_FLAGS_APPEND)"
C_MAKEENV_STATIC += CPPFLAGS="$(_BDEFS_FLAGS_APPEND) $(filter-out -D_GNU_SOURCE,$(CPPFLAGS))"
C_MAKEENV_STATIC += LDFLAGS="$(LDFLAGS) $(_BDEFS_FLAGS_APPEND)"
C_MAKEENV_STATIC +=

X_C_MAKEENV_STATIC = $(C_MAKEENV_STATIC) CC="$(TARGET_CC)"

X_CONFIGURE_OPTS =
X_CONFIGURE_OPTS += --prefix=/ --exec-prefix=/
X_CONFIGURE_OPTS += --libexecdir=/lib
X_CONFIGURE_OPTS += --build="$(shell $(TARGET_CC) -dumpmachine)"
X_CONFIGURE_OPTS += --host="$(shell gcc -dumpmachine)"
X_CONFIGURE_OPTS += --disable-nls


X_WGET  ?= wget
X_TAR   ?= tar

DL_DIR = $(S)/src/download

# F_FETCH_FILE ( uri, dst )
F_FETCH_FILE = mkdir -p -- '$(dir $(2))' && $(X_WGET) -O '$(2)' '$(1)'

DOSED_TMPFILE = sed -r -i $(@).tmp -e

F_FEATURE_CONFIGURE_OPTS = \
	$(foreach feat,$($(1)_FEATURES_ENABLE), --enable-$(feat)) \
	$(foreach feat,$($(1)_FEATURES_WITH), --with-$(feat)) \
	$(foreach feat,$($(1)_FEATURES_DISABLE), --disable-$(feat)) \
	$(foreach feat,$($(1)_FEATURES_WITHOUT), --without-$(feat)) \


define DO_DEFAULT_UNTAR
	mkdir -p -- $(@D)
	rm -rf -- $(@)

	$(X_TAR) xa -C $(@D) -f $<
	cd $(@)
endef
