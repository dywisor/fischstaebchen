# prog defs
$(foreach prog,$(BINDIR_PROGS),$(eval $(call C_PROG_DEF_WITH_BINDIR_INSTALL,$(prog))))
$(foreach prog,$(SBINDIR_PROGS),$(eval $(call C_PROG_DEF_WITH_SBINDIR_INSTALL,$(prog))))

# preinit
PROG_PREINIT_MAIN_SRC       = $(call f_get_objectsv,initramfs/main/preinit)
PROG_PREINIT_SRC_DEP       += $(call get_c_headers,INITRAMFS)
PROG_PREINIT_SRC_DEP       += $(call get_c_objects,INITRAMFS)
PROG_PREINIT_NOLIB_SRC_DEP += $(call get_c_objects,COMMON ZRAM)
PROG_PREINIT_NOLIB_SRC_DEP += $(call get_c_headers,COMMON ZRAM)

$(eval $(call C_PROG_DEF,preinit))
PHONY += $(addsuffix -preinit,install uninstall)
install-preinit:
	$(DOEXE) -- $(PROG_PREINIT_BUILD) $(DESTDIR:/=)/rdinit
	$(DOSYM) -- rdinit $(DESTDIR:/=)/init

uninstall-preinit:
	$(RMF) -- $(DESTDIR:/=)/rdinit
	$(RMF) -- $(DESTDIR:/=)/init


# init
PROG_INIT_MAIN_SRC       = $(call f_get_objectsv,initramfs/main/init)
PROG_INIT_SRC_DEP       += $(call get_c_objects,APP_CREATE_USER_TMPDIRS)
PROG_INIT_SRC_DEP       += $(call get_c_headers,APP_CREATE_USER_TMPDIRS)
PROG_INIT_SRC_DEP       += $(call get_c_headers,INITRAMFS)
PROG_INIT_SRC_DEP       += $(call get_c_objects,INITRAMFS)
PROG_INIT_NOLIB_SRC_DEP += $(call get_c_objects,COMMON ZRAM)
PROG_INIT_NOLIB_SRC_DEP += $(call get_c_headers,COMMON ZRAM)

PROG_INIT_DEST           = rdinit

$(eval $(call C_PROG_DEF,init))
PHONY += $(addsuffix -init,install uninstall)
install-init:
	$(DOEXE) -- $(PROG_INIT_BUILD)  $(DESTDIR:/=)/$(PROG_INIT_DEST)
	$(DOSYM) -- ./$(PROG_INIT_DEST) $(DESTDIR:/=)/init

uninstall-init:
	$(RMF) -- $(DESTDIR:/=)/$(PROG_INIT_DEST)
	$(RMF) -- $(DESTDIR:/=)/init


# aufs-overlayfs-wrapper
#  "logically" broken - does not handle workdir=
ifeq ($(BUILD_BROKEN),1)
$(eval $(call C_PROG_DEF,aufs-overlayfs-wrapper))

ifeq ($(INSTALL_BROKEN),1)
$(eval $(call DEF_INSTALL_TARGET,aufs-overlayfs-wrapper))

install-aufs-overlayfs-wrapper:
	$(DOEXE) -- $(PROG_AUFS_OVERLAYFS_WRAPPER_BUILD) \
		$(DESTDIR:/=)$(BINDIR)/aufs-overlayfs-wrapper

uninstall-aufs-overlayfs-wrapper:
	$(RMF) -- $(DESTDIR:/=)$(BINDIR)/aufs-overlayfs-wrapper

endif
endif
