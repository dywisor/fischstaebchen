HOOKDIR = $(D:/=)/$(OVERLAY_HOOKDIR:/=)
PHASES  =

PHASES += subtree-mount
PHASES += populate-newroot
PHASES += setup-newroot
PHASES += finalize-newroot

# not ideal, but at least way faster than a shell for-loop
#  need dep on $$(HOOKDIR)/$$(p), else make tries to remove this dir
all: $(foreach p,$(PHASES),\
	$(HOOKDIR)/$(p).d \
	$(HOOKDIR)/$(p).d/01-newroot_hooks_pre.sh \
	$(HOOKDIR)/$(p).d/50-newroot_hooks.sh \
	$(HOOKDIR)/$(p).d/99-newroot_hooks_post.sh)




define _f_genhook
	{ \
		printf '%s\n' '<% if ALLOW_NEWROOT_HOOKS %>' && \
		printf '%s\n' "newroot_run_hooks \"\$${PHASE}$(1)\"" && \
		printf '%s\n' '<% endif %>'; \
	}
endef

$(HOOKDIR)/%.d:
	mkdir -p -- $(@)

$(HOOKDIR)/%.d/01-newroot_hooks_pre.sh: $(HOOKDIR)/%.d FORCE
	$(call _f_genhook,/pre) | $(RUN_METASH) -O $@

$(HOOKDIR)/%.d/50-newroot_hooks.sh: $(HOOKDIR)/%.d FORCE
	$(call _f_genhook,) | $(RUN_METASH) -O $@

$(HOOKDIR)/%.d/99-newroot_hooks_post.sh: $(HOOKDIR)/%.d FORCE
	$(call _f_genhook,/post) | $(RUN_METASH) -O $@



FORCE:

.PHONY: all
