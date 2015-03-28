PHONY += $(_ALL_OVERLAY_TARGETS)
$(_ALL_OVERLAY_TARGETS): overlay-%: $(O)/.stamp_overlay_%

PHONY += _overlay_fs_fixup
_overlay_fs_fixup: | $(_ALL_OVERLAY_TARGETS) _basedep_clean
	find $(OVERLAY_O_INITDIR)/ -type f \
		\( \
			-name README -or -name 'README.*' -or -name '.keep*' \
		\) -print -delete

	find $(OVERLAY_O_HOOKDIR) -type f \
		\( -empty -or -not -exec grep -qEv -- '(^[#]|^\s*$$)' '{}' \; \) \
		-print -delete

	find $(OVERLAY_O_HOOKDIR) \
		-mindepth 1 -maxdepth 1 -type d -name '*.d' -empty -print -delete

	find $(OVERLAY_O_HOOKDIR) -maxdepth 1 -type f \
		-exec $(CHMOD) -- '$(EXEMODE)' '{}' +

	find $(OVERLAY_O_HOOKDIR) -mindepth 2 -type f \
		-exec $(CHMOD) -- '$(INSMODE)' '{}' +

	if test -f '$(OVERLAY_O_HOOKDIR)/_stdmux.sh'; then \
		$(CHMOD) -- '$(EXEMODE)' '$(OVERLAY_O_HOOKDIR)/_stdmux.sh' && \
		find $(OVERLAY_O_HOOKDIR) \
			-mindepth 1 -maxdepth 1 \
			-type d -name '*.d' -not -name 'stagedive*' | \
				sed -r -e 's=^(.*[/])?([^/]+)[.]d$$=\2=' | \
					xargs -r -n 1 -I '{phase}' \
						$(SHELL) -c '\
							test -e "$(OVERLAY_O_HOOKDIR)/{phase}" || \
							test -h "$(OVERLAY_O_HOOKDIR)/{phase}" || \
							$(DOSYM) -- _stdmux.sh "$(OVERLAY_O_HOOKDIR)/{phase}";\
						'; \
	fi

	set -e; for d in $(addprefix $(OVERLAY_O),\
		/bin  /usr/bin \
		/sbin /usr/sbin \
	); do \
		if test -d "$${d}" && test ! -h "$${d}"; then \
			find "$${d}" -type f -not -executable \
				-exec $(CHMOD) -- '$(EXEMODE)' '{}' +; \
		fi; \
	done


PHONY += overlay
overlay: \
	$(_ALL_OVERLAY_TARGETS) \
	$(OVERLAY_O_FUNCTIONS_DIR)/functions.sh \
	$(OVERLAY_O_ENVFILES_DIR)/env.sh \
	_overlay_fs_fixup | _basedep_clean

	$(RM) -f -- '$(OVERLAY_O_WANTSED).append'
	$(MKDIRP) -- '$(dir $(OVERLAY_O_WANTSED))'
	: > '$(OVERLAY_O_WANTSED).append'


	$(DOINS) -- '$(OVERLAY_O_ENVFILES_DIR)/env.sh' \
		'$(OVERLAY_O_ENVFILE_FILE)'

	$(DOINS) -- '$(OVERLAY_O_FUNCTIONS_DIR)/functions.sh' \
		'$(OVERLAY_O_FUNCTIONS_FILE)'

	{ \
		printf  '%s\n' '$(OVERLAY_O_ENVFILE_FILE)' && \
		printf  '%s\n' '$(OVERLAY_O_FUNCTIONS_FILE)'; \
	} >> '$(OVERLAY_O_WANTSED).append'

	# FIXME: useless || should want-sed these files elsewhere
	set -e; for f in $(addprefix $(OVERLAY_O),\
		/telinit \
		/etc/udhcpc.script \
		/usr/share/udhcpc/default.script \
	); do \
		if test -f "$${f}" && test ! -h "$${f}"; then \
			printf "%s\n" "$${f}" >> '$(OVERLAY_O_WANTSED).append'; \
			$(CHMOD) -- $(EXEMODE) "$${f}"; \
		fi; \
	done

	find \
		'$(OVERLAY_O_INITDIR)/' \
		'$(OVERLAY_O_HOOKDIR)/' \
		-type f >> '$(OVERLAY_O_WANTSED).append'

	sed -e 's=^=$(OVERLAY_O:/=)=' -- \
		'$(OVERLAY_O_WANTSED)' > '$(OVERLAY_O_WANTSED).abspath'

	sort -u \
		'$(OVERLAY_O_WANTSED).abspath' '$(OVERLAY_O_WANTSED).append' \
		> '$(OVERLAY_O_WANTSED).out'

	# sed-edit files
	< '$(OVERLAY_O_WANTSED).out' xargs -r $(OVERLAY_SED_EDIT)

	# check for leftover metavars
	< '$(OVERLAY_O_WANTSED).out' xargs -r -n 1 -I '{f}' \
		$(SHELL) -c '\
			if grep -E -- "@@[A-Z_]+@@" "{f}"; then \
				exit 1; \
			fi;'
