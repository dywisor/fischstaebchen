FUNCTIONS_SRCDIR := $(S)/src

FUNCTIONS_O_ROOT     := $(O)/functions
FUNCTIONS_O_COMBINED := $(FUNCTIONS_O_ROOT)/combined
FUNCTIONS_O_METASH   := $(FUNCTIONS_O_ROOT)/metash
FUNCTIONS_O_TMP      := $(FUNCTIONS_O_ROOT)/tmp

$(FUNCTIONS_O_METASH)/%/.stamp_metash: $(FUNCTIONS_SRCDIR)/% | _basedep_clean
	$(RMF) -r -- $(@D)
	$(MKDIRP) -- $(@D)

	$(call f_metash_do_build,\
		$(FUNCTIONS_O_TMP)/metash/$(*),\
		$(<),\
		$(FUNCTIONS_O_METASH)/$(*))

	touch $(@)

$(FUNCTIONS_O_COMBINED)/%.sh: $(FUNCTIONS_O_METASH)/%/.stamp_metash | _basedep_clean
	$(MKDIRP) -- $(@D)
	$(call f_combine_script_file_dir,$(<D),$(@))

# no sed-edit for these files, FUNCTIONS_O==FUNCTIONS_O_COMBINED
FUNCTIONS_O := $(FUNCTIONS_O_COMBINED)
