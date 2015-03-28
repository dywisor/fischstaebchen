ifeq ($(__PRJROOT),)
$(error __PRJROOT is not set)
endif

BUILDSCRIPTS_DIR := $(__PRJROOT)/scripts

X_EXTRACT_HASHBANG  = $(AWKF) $(BUILDSCRIPTS_DIR)/extract-hashbang
X_SHELLCHECK        = $(SHELL) $(BUILDSCRIPTS_DIR)/run-shellcheck
X_METASH            = $(PYTHON_PREFER3) $(BUILDSCRIPTS_DIR)/metash
X_MERGE_SCRIPTFILES = $(PYTHON_PREFER3) $(BUILDSCRIPTS_DIR)/merge-scriptfiles
