ifeq ($(__PRJROOT),)
$(error __PRJROOT is not set)
endif

BUILDSCRIPTS_DIR := $(__PRJROOT)/scripts

X_SHELLCHECK = $(BUILDSCRIPTS_DIR)/run-shellcheck
