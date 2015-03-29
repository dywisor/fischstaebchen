INITRAMFS_COMPONENTS += shellfunc

SHFUNC_O             = $(O)/shellfunc
INITRAMFS_SHFUNC_SRC = $(__PRJROOT)/shellfunc

SHFUNC_MAKEOPTS  = -C $(INITRAMFS_SHFUNC_SRC)
SHFUNC_MAKEOPTS += O='$(O)' SHFUNC_O='$(SHFUNC_O)'
SHFUNC_MAKEOPTS += SHFUNCS='$(INITRAMFS_SHFUNCS)'
SHFUNC_MAKEOPTS += SHFUNC_LIBDIR='$(SHFUNC_LIBDIR)'

$(SHFUNC_O): $(INITRAMFS_SHFUNC_SRC)/Makefile
	$(MAKE) $(SHFUNC_MAKEOPTS)


PHONY += shellfunc
shellfunc: $(SHFUNC_O)


PHONY += install-shellfunc
install-shellfunc: shellfunc
	$(MAKE) $(SHFUNC_MAKEOPTS) DESTDIR=$(INITRAMFS) install

PHONY += uninstall-shellfunc
uninstall-shellfunc:
	test -n '$(INITRAMFS)'

	test ! -d '$(SHFUNC_O)' || \
	cd $(SHFUNC_O) && find ./ -type f -print0 | \
		xargs -r -0 -I '{R}' rm -vf -- '$(INITRAMFS)/{R}'

PHONY += reinstall-shellfunc
reinstall-shellfunc: uninstall-shellfunc
	$(MAKE) $(SHFUNC_MAKEOPTS) DESTDIR=$(INITRAMFS) install
