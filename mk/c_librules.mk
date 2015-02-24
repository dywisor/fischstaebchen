LIBFISCHSTAEBCHEN_SO_VERSION = 0

# _C_LIB_VARS_DEF ( make_name, lib_name )
#
define _C_LIB_VARS_DEF
$(eval LIB$(1)_SO_VERSION            = $(LIBFISCHSTAEBCHEN_SO_VERSION))
$(eval LIB$(1)_SO_NAME               = $(2))
$(eval LIB$(1)_SO_LIBNAME            = lib$(LIB$(1)_SO_NAME))
$(eval LIB$(1)_SO_FILENAME           = $(LIB$(1)_SO_LIBNAME).so)
$(eval LIB$(1)_SO_FILENAME_VERSIONED = \
	$(LIB$(1)_SO_FILENAME).$(LIB$(1)_SO_VERSION))
endef

# _C_LIB_OBJ_STD_DEF ( make_name, lib_name, <components> )
#
define _C_LIB_OBJ_STD_DEF
$(eval LIB$(1)_OBJECTS += $(call get_c_objects,$(3)))
$(eval LIB$(1)_HEADERS += $(call get_c_headers,$(3)))
$(eval LIB$(1)_SRCDEPS  = $(LIB$(1)_OBJECTS) $$(LIB$(1)_HEADERS))

# $(OLIB)/lib<name>.so.<version>
$(OLIB)/$(LIB$(1)_SO_FILENAME_VERSIONED): $(LIB$(1)_SRCDEPS)
	$(MKDIRP) -- $$(@D)
	$(COMPILE_C_SHARED) -Wl,-soname,$$(@F) $$(filter-out %.h,$$^) -o $$@
	$(TARGET_STRIP_IF_REQUESTED) -s $$@

# $(OLIB)/lib<name>.so
$(OLIB)/$(LIB$(1)_SO_FILENAME): $(OLIB)/$(LIB$(1)_SO_FILENAME_VERSIONED)
	$(DOSYM) -- $$(<F) $$(@)

# lib<name> (phony)
PHONY     += $(LIB$(1)_SO_LIBNAME)
LIB_NAMES += $(LIB$(1)_SO_LIBNAME)

$(LIB$(1)_SO_LIBNAME): \
	$(OLIB)/$(LIB$(1)_SO_FILENAME_VERSIONED) \
	$(OLIB)/$(LIB$(1)_SO_FILENAME)

# install-lib<name>
$(call DEF_INSTALL_TARGET,$(LIB$(1)_SO_LIBNAME))

ifeq ($(LIBFISCHSTAEBCHEN_VERSIONED),1)
install-$(LIB$(1)_SO_LIBNAME):
	$(DOEXE) $(OLIB)/$(LIB$(1)_SO_FILENAME_VERSIONED) \
		$(DESTDIR)$(LIBDIR)/$(LIB$(1)_SO_FILENAME_VERSIONED)
	$(DOSYM) -- $(LIB$(1)_SO_FILENAME_VERSIONED) \
		$(DESTDIR)$(LIBDIR)/$(LIB$(1)_SO_FILENAME)

else
install-$(LIB$(1)_SO_LIBNAME):
	$(DOEXE) $(OLIB)/$(LIB$(1)_SO_FILENAME_VERSIONED) \
		$(DESTDIR)$(LIBDIR)/$(LIB$(1)_SO_FILENAME)
endif

uninstall-$(LIB$(1)_SO_LIBNAME):
	$(RMF) -- $(DESTDIR)$(LIBDIR)/$(LIB$(1)_SO_FILENAME_VERSIONED)
	$(RMF) -- $(DESTDIR)$(LIBDIR)/$(LIB$(1)_SO_FILENAME)

endef
# end lib

# _C_LIB_DEF ( make_name, lib_name, <components> )
#
define _C_LIB_DEF
$(call _C_LIB_VARS_DEF,$(1),$(2))
$(call _C_LIB_OBJ_STD_DEF,$(1),$(2),$(3))
endef

# C_LIB_VARS_DEF ( lib_name )
C_LIB_VARS_DEF = $(call _C_LIB_VARS_DEF,$(call _f_convert_name,$1),$(1))
# C_LIB_DEF ( lib_name, <components> )
C_LIB_DEF      = $(call _C_LIB_DEF,$(call _f_convert_name,$1),$(1),$(2))
