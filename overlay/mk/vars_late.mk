ifeq ($(OVERLAY_NAMES),)
$(error OVERLAY_NAMES is not set)
endif


_ALL_OVERLAY_TARGETS := $(addprefix overlay-,$(_ALL_OVERLAYS))
