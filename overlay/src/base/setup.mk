all: $(D)/functions.sh _regsed

$(D)/functions.sh:
	ln -f -s -- '.$(OVERLAY_FUNCTIONS_FILE)' '$(@)'

_regsed:
	$(WANTSED) /bin/net-setup

.PHONY: all _regsed
