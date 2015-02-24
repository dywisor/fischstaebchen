ifeq ($(COMPILE_C),)
$(error c_rules.mk must be included before obj_defs.mk)
endif

ifeq ($(SRCDIR),)
$(error SRCDIR is not set)
endif

include $(SRCDIR)/obj_defs.mk
