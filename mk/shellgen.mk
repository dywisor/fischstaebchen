ifeq ($(BUILDSCRIPTS_DIR),)
$(error build_scripts must be included before this file)
endif



METASH_VDEF_OPTS = -F '$(__PRJROOT)/misc/metash_vdef'
ifneq ($(METASH_DEFAULTS_FILE),)
METASH_VDEF_OPTS += -F '$(METASH_DEFAULTS_FILE)'
endif

METASH_VDEF_OPTS += $(foreach f,$(METASH_DEFAULTS_FILES),-F '$(f)')

RUN_METASH = $(X_METASH) $(METASH_VDEF_OPTS) $(METASH_OPTS)

# f_run_metash_convert_file ( srcfile, dstfile, [extra_opts] )
define f_run_metash_convert_file
	$(RUN_METASH) $(3) -O '$(strip $(2))' '$(strip $(1))'
endef

# f_run_metash_convert_dir ( srcdir, outdir, ["--stdin"], [extra_opts] )
define f_run_metash_convert_dir
	$(SHELL) $(BUILDSCRIPTS_DIR)/run-metash $(3) \
		'$(strip $(1))' '$(strip $(2))' -- $(RUN_METASH) $(4)
endef

# f_run_metash_convert_dir_ifexist ( srcdir, outdir, ["--stdin"], [extra_opts] )
define f_run_metash_convert_dir_ifexist
	test ! -e '$(strip $(1))' || \
		$(call f_run_metash_convert_dir,$(1),$(2),$(3),$(4))
endef

# f_run_shell_syntax_check_recursive ( dir )
#
define f_run_shell_syntax_check_recursive
	find $(strip $(1)) -type f -name '*.sh' | xargs -r -n 1 $(SHELL) -n

	{ \
		set -e; \
		for f in $$( find $(strip $(1)) -type f -not -name '*.sh' ); do \
			if \
				$(X_EXTRACT_HASHBANG) "$${f}" | \
					grep -q -E -- "^(@@X?SHELL@@|(d?a)?sh)$$"; \
			then \
				$(SHELL) -n "$${f}" || { printf '>>> %s <<<\n' "$${f}"; exit 1; } \
			fi; \
		done; \
	}

endef

# _f_metash_do_build ( tmpdir, srcdir, dstdir )
define _f_metash_do_build
	$(RMF) -r -- '$(1)'
	$(MKDIRP) -- '$(1)'

	$(call f_run_metash_convert_dir_ifexist,$(2),$(1))

	$(call f_run_shell_syntax_check_recursive,$(1))

	$(call f_copy_tree,$(1),$(3))
endef

# f_metash_do_build(...)
define f_metash_do_build
	$(call _f_metash_do_build,$(strip $(1)),$(strip $(2)),$(strip $(3)))
endef

# _f_combine_script_files ( srcroot, dstdir )
#
#   srcroot/<name>/<files> => dstdir/<name>.sh
#
define _f_combine_script_files
	# scan for files not supported (yet?)
	! { find '$(1)/' -type f -name '*.in' | grep -- .; }

	$(MKDIRP) -- '$(2)'

	# create per-directory functions files
	{ \
		set -e; \
		D='$(2)'; \
		\
		for src in '$(1)/'*; do \
			if test -e "$${src}"; then \
				name="$${src##*/}"; \
				\
				if test -h "$${src}" || test ! -d "$${src}"; then \
					printf  "%s\n"  "Cannot handle non-dir $${name} ($${src})!"; \
					exit 9; \
				else \
					$(X_MERGE_SCRIPTFILES) \
						-O "$${D}/$${name}.sh" \
						$$( find "$${src}/" -type f -name '*.sh' | sort -V ); \
				fi; \
			fi; \
		done; \
	}
endef

# f_combine_script_files(...)
define f_combine_script_files
	$(call _f_combine_script_files,$(strip $(1)),$(strip $(2)))
endef
