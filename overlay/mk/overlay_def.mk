OVERLAY_SETUP_ENV  =
OVERLAY_SETUP_ENV += OVERLAY='$(OVERLAY_O)'
OVERLAY_SETUP_ENV += D='$(OVERLAY_O)'
OVERLAY_SETUP_ENV += OVERLAY_INITDIR='$(OVERLAY_INITDIR_REL)'
OVERLAY_SETUP_ENV += OVERLAY_METASCRIPTDIR='$(OVERLAY_METASCRIPTDIR_REL)'
OVERLAY_SETUP_ENV += OVERLAY_HOOKDIR='$(OVERLAY_HOOKDIR_REL)'
OVERLAY_SETUP_ENV += OVERLAY_FUNCTIONS_FILE='$(OVERLAY_FUNCTIONSFILE_REL)'
OVERLAY_SETUP_ENV += OVERLAY_O_FUNCTIONS_DIR='$(OVERLAY_O_FUNCTIONS_DIR)'
OVERLAY_SETUP_ENV += OVERLAY_ENV_FILE='$(OVERLAY_ENVFILE_REL)'
OVERLAY_SETUP_ENV += OVERLAY_O_WANTSED='$(OVERLAY_O_WANTSED)'
OVERLAY_SETUP_ENV += WANTSED='>> $(OVERLAY_O_WANTSED) printf "%s\n"'
OVERLAY_SETUP_ENV += RUN_METASH='$(RUN_METASH)'
##OVERLAY_SETUP_ENV += OVERLAY_FAKEROOTDIR='$(OVERLAY_O_FAKEROOTD)'
#OVERLAY_SETUP_ENV +=



$(OVERLAY_O): FORCE | _basedep_clean
	$(DODIR) -- \
		$(OVERLAY_O) \
		$(OVERLAY_O_INITDIR) \
		$(OVERLAY_O_METASCRIPTDIR) \
		$(OVERLAY_O_HOOKDIR) \
		$(addprefix $(OVERLAY_O_FUNCTIONS_DIR)/,src combined) \
		$(dir $(OVERLAY_O_FUNCTIONS_FILE)) \
		$(OVERLAY_O_ENVFILES_DIR)/src \
		$(dir $(OVERLAY_O_ENVFILE_FILE)) \
		$(OVERLAY_O_FAKEROOTD) \
		$(dir $(OVERLAY_O_WANTSED)) \
		$(OVERLAY_O_TMPROOT)

	touch -- '$(OVERLAY_O_WANTSED)'


# _OVERLAY_DEF ( make_name, overlay_name )
#
#  (could use a <target>: %: ... rule for that)
#
define _OVERLAY_DEF
$(eval OVERLAY_$(1)_SRCDIR = $(OVERLAY_SRCDIR)/$(2))

$(O)/.stamp_overlay_metascript_$(2): \
	$(O)/.stamp_overlay_metascript_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	$(call f_copy_tree_ifexist,$$(<)/metascript,$(OVERLAY_O_METASCRIPTDIR))


ifeq ($(X_STATIC_SHELL_FILES),1)
$(O)/.stamp_overlay_env_$(2): \
	$(O)/.stamp_overlay_env_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	set -e; \
	if test -f '$$(<)/static/env.sh'; then \
		$(CP) -vL -- '$$(<)/static/env.sh' \
			'$(OVERLAY_O_ENVFILES_DIR)/src/$$(*).sh'; \
	elif test -e '$$(<)/static/env.sh'; then \
		false not-a-file; \
	else \
		touch -- '$(OVERLAY_O_ENVFILES_DIR)/src/$$(*).sh'; \
	fi

	touch '$$(@)'

$(O)/.stamp_overlay_functions_$(2): \
	$(O)/.stamp_overlay_functions_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	$(call f_copy_tree_ifexist,$$(<)/static/functions,$(OVERLAY_O_FUNCTIONS_DIR)/src)
	touch '$$(@)'

$(O)/.stamp_overlay_hooks_$(2): \
	$(O)/.stamp_overlay_hooks_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	$(call f_copy_tree_ifexist,$$(<)/static/hooks,$(OVERLAY_O_HOOKDIR))
	touch '$$(@)'

else
$(O)/.stamp_overlay_env_$(2): \
	$(O)/.stamp_overlay_env_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	set -e; \
	if test -f '$$(<)/env.sh'; then \
		$(RUN_METASH) \
			-O '$(OVERLAY_O_ENVFILES_DIR)/src/$$(*).sh' \
			'$$(<)/env.sh'; \
	elif test -e '$$(<)/env.sh'; then \
		false not-a-file; \
	else \
		touch -- '$(OVERLAY_O_ENVFILES_DIR)/src/$$(*).sh'; \
	fi

	touch '$$(@)'

$(O)/.stamp_overlay_functions_$(2): \
	$(O)/.stamp_overlay_functions_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	$(call f_copy_tree_ifexist,$$(<)/functions,$(OVERLAY_O_FUNCTIONS_DIR)/src)

	$(RMF) -r -- '$(OVERLAY_O_TMPROOT)/$$(*)/functions'
	$(MKDIRP) -- '$(OVERLAY_O_TMPROOT)/$$(*)/functions'

	$(call f_run_metash_convert_dir_ifexist,\
		$$(<)/functions,$(OVERLAY_O_TMPROOT)/$$(*)/functions)

	find $(OVERLAY_O_TMPROOT)/$$(*)/functions -type f -name '*.sh' | \
		xargs -n 1 $(SHELL) -n

	$(call f_copy_tree,$(OVERLAY_O_TMPROOT)/$$(*)/functions,$(OVERLAY_O_FUNCTIONS_DIR)/src)
	touch '$$(@)'

$(O)/.stamp_overlay_hooks_$(2): \
	$(O)/.stamp_overlay_hooks_%: $(OVERLAY_$(1)_SRCDIR) | $(OVERLAY_O) _basedep_clean

	$(call f_copy_tree_ifexist,$$(<)/static/hooks,$(OVERLAY_O_HOOKDIR))

	$(RMF) -r -- '$(OVERLAY_O_TMPROOT)/$$(*)/hooks'
	$(MKDIRP) -- '$(OVERLAY_O_TMPROOT)/$$(*)/hooks'

	$(call f_run_metash_convert_dir_ifexist,\
		$$(<)/hooks,$(OVERLAY_O_TMPROOT)/$$(*)/hooks)

	$(call f_copy_tree,$(OVERLAY_O_TMPROOT)/$$(*)/hooks,$(OVERLAY_O_HOOKDIR))
	touch '$$(@)'

endif


$(O)/.stamp_overlay_$(2): \
	$(O)/.stamp_overlay_%: \
		$(OVERLAY_$(1)_SRCDIR) $(OVERLAY_O) \
		$(O)/.stamp_overlay_metascript_$(2) \
		$(O)/.stamp_overlay_functions_$(2) \
		$(O)/.stamp_overlay_hooks_$(2) \
		$(O)/.stamp_overlay_env_$(2) | _basedep_clean


	$$(call f_copy_tree_ifexist,$$(<)/rootfs,$(OVERLAY_O))

	{ \
		set -e; \
		set -- \
			$$(foreach p,$$(wildcard $$(<)/rootfs/bin/*),$$(p:$$(<)/rootfs/%=/%)) \
		; \
		while [ $$$${#} -gt 0 ]; do \
			printf '%s\n' "$$$${1}"; shift; \
		done >> '$(OVERLAY_O_WANTSED)'; \
	}

	if test -d '$$(<)/fakeroot-setup'; then \
		$(call f_copy_tree,$$(<)/fakeroot-setup,$(OVERLAY_O_FAKEROOTD)); \
		\
	elif test -f '$$(<)/fakeroot-setup'; then \
		$(DOINS) -- '$$(<)/fakeroot-setup' '$(OVERLAY_O_FAKEROOTD)/$$(*)'; \
	fi

	if test -f '$$(<)/fakeroot-setup.sh'; then \
		$(DOINS) -- '$$(<)/fakeroot-setup.sh' '$(OVERLAY_O_FAKEROOTD)/$$(*).sh'; \
	fi

	if test -f '$$(<)/setup.sh'; then \
		cd '$(OVERLAY_O)' && \
		$(OVERLAY_SETUP_ENV) $(SHELL) '$$(<)/setup.sh' \
			'$(OVERLAY_O)' '$$(<)' '$$(*)'; \
	fi

	+if test -f '$$(<)/setup.mk'; then \
		$(MAKE) -B -f '$$(<)/setup.mk' -C '$(OVERLAY_O)' \
			$(OVERLAY_SETUP_ENV) \
			D='$(OVERLAY_O)' S='$$(<)' NAME='$$(*)'; \
	fi

	touch $$@


endef

# OVERLAY_DEF ( overlay_name )
OVERLAY_DEF = $(call _OVERLAY_DEF,$(call f_convert_name,$(1)),$(1))
