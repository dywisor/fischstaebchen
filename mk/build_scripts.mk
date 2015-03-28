ifeq ($(__PRJROOT),)
$(error __PRJROOT is not set)
endif

BUILDSCRIPTS_DIR := $(__PRJROOT)/scripts
