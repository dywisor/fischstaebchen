all: $(D)/functions.sh

$(D)/functions.sh:
	ln -f -s -- '.$(OVERLAY_FUNCTIONS_FILE)' '$(@)'

.PHONY: all
