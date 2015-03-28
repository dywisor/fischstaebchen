OVERLAY_FUNCTIONS_ORDER  =
OVERLAY_FUNCTIONS_ORDER += base
OVERLAY_FUNCTIONS_ORDER += core
OVERLAY_FUNCTIONS_ORDER += rootdir
OVERLAY_FUNCTIONS_ORDER += phaseout
OVERLAY_FUNCTIONS_ORDER += union_mount
OVERLAY_FUNCTIONS_ORDER += newroot
OVERLAY_FUNCTIONS_ORDER += newroot_hooks
OVERLAY_FUNCTIONS_ORDER += stagedive
OVERLAY_FUNCTIONS_ORDER += dotfiles

$(OVERLAY_O_TMPROOT)/include_order: \
	$(OVERLAY_O) FORCE | $(_ALL_OVERLAY_TARGETS) _basedep_clean

	$(RMF) -- $(@) $(@).new $(@).in

	set -e; \
	for item in $(OVERLAY_FUNCTIONS_ORDER); do \
		printf  '%s\n' "$${item}"; \
	done > $(@).new

	set -e; \
	{ \
		{ \
			find '$(OVERLAY_O_FUNCTIONS_DIR)/src/' \
				-mindepth 1 -maxdepth 1 -type d | \
					sed -e 's=^$(OVERLAY_O_FUNCTIONS_DIR)/src/=='; \
		} && { \
			find '$(OVERLAY_O_ENVFILES_DIR)/src/' -type f -name '*.sh' | \
				sed -r \
					-e 's=[.]sh$$==' \
					-e 's=^$(OVERLAY_O_ENVFILES_DIR)/src/=='; \
		} \
	} | sort -u > '$(@).in'

	test -s '$(@).in'

	set -e; \
	while read -r k; do \
		grep -Fx -- "$${k}" '$(@).new' || \
			printf '%s\n' "$${k}" >> '$(@).new'; \
	done < '$(@).in'

	$(RM) -- '$(@).in'
	mv -f -- '$(@).new' '$(@)'


PHONY += _gen_functions_clean
_gen_functions_clean: $(OVERLAY_O) FORCE | $(_ALL_OVERLAY_TARGETS) _basedep_clean
	test -n '$(OVERLAY_O_FUNCTIONS_DIR)'
	$(RM) -f  -- '$(OVERLAY_O_FUNCTIONS_DIR)/functions.sh'
	$(RM) -rf -- '$(OVERLAY_O_FUNCTIONS_DIR)/combined'
	$(MKDIRP) -- '$(OVERLAY_O_FUNCTIONS_DIR)/combined'

	set -f; find '$(OVERLAY_O_FUNCTIONS_DIR)/src/' -type f \
		\( \
			-name '00-include.sh' -or -name '00-includes.sh' \
			$(foreach e,*README* AUTHORS COPYING ACKNOWLEDGEMENTS,\
				-or -name '$(e)') \
		\) -delete

	find '$(OVERLAY_O_FUNCTIONS_DIR)/src/' -depth -type d -empty -delete

PHONY += _gen_functions_combine
_gen_functions_combine: \
	$(OVERLAY_O) _gen_functions_clean FORCE | $(_ALL_OVERLAY_TARGETS) _basedep_clean

	$(call f_combine_script_files,\
		$(OVERLAY_O_FUNCTIONS_DIR)/src,\
		$(OVERLAY_O_FUNCTIONS_DIR)/combined)



# _INCLUDE_FILES_FROM ( include_order_file, srcdir, dstfile )
#
define _INCLUDE_FILES_FROM
#$$(foreach name,$$(shell cat $$(1)),...)
	{ \
		set -e; \
		\
		set --; \
		while read -r name; do \
			src="$(2)/$${name}.sh"; \
			\
			if test -f "$${src}" && test -s "$${src}"; then \
				printf  '>>> %s\n' "$${name}"; \
				set -- "$${@}" "$${src}"; \
			fi; \
		done < '$(1)' && \
		\
		$(X_MERGE_SCRIPTFILES) -O '$(3)' "$${@}" && \
		$(SHELL) -n '$(3)'; \
	}
endef


$(OVERLAY_O_ENVFILES_DIR)/env.sh: \
	$(OVERLAY_O_TMPROOT)/include_order $(OVERLAY_O) \
	FORCE | $(_ALL_OVERLAY_TARGETS) _basedep_clean

	$(MKDIRP) -- $(@D)

	$(call _INCLUDE_FILES_FROM,$(<),$(OVERLAY_O_ENVFILES_DIR)/src,$(@))


$(OVERLAY_O_FUNCTIONS_DIR)/functions.sh: \
	$(OVERLAY_O_TMPROOT)/include_order $(OVERLAY_O) \
	$(addprefix _gen_functions_,clean combine) \
	FORCE | $(_ALL_OVERLAY_TARGETS) _basedep_clean

	$(MKDIRP) -- $(@D)

	# merge the combined files, functions.sh
	$(call _INCLUDE_FILES_FROM,$(<),$(OVERLAY_O_FUNCTIONS_DIR)/combined,$(@))
