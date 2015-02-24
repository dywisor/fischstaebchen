INITRAMFS_COMPONENTS += pv

PV_VERSION = 1.5.7
PV_SOURCE  = pv-$(PV_VERSION)

PV_SRC_URI = http://www.ivarch.com/programs/sources/$(PV_SOURCE).tar.bz2

PV_SRC := $(DL_DIR)/$(notdir $(PV_SRC_URI))
PV_O   := $(O)/pv/$(PV_SOURCE)

PHONY += pv
pv: $(PV_O)/pv

PHONY += install-pv
install-pv: $(PV_O)/pv
	$(DOEXE) -- $(<) $(INITRAMFS)/bin/pv

$(PV_SRC):
	$(call F_FETCH_FILE,$(PV_SRC_URI),$(PV_SRC))



$(PV_O): $(PV_SRC) FORCE
	$(DO_DEFAULT_UNTAR)


$(PV_O)/pv: $(PV_O)
	cd $(@D) && \
		$(X_C_MAKEENV_STATIC) $(SHELL) ./configure \
			$(X_CONFIGURE_OPTS) \
			--disable-debugging --disable-profiling

	cd $(@D) && $(X_C_MAKEENV_STATIC) $(MAKE) pv
	$(CROSS_COMPILE)strip -s $@
