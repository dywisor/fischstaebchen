#!/bin/sh
#  env vars for both staging and target
#
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_ARCH_BOOTSTRAP_SRCDIR="${__DIR__}"

ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR="${STAGEDIVE_RUNTIME_CONFDIR:?}/archstrap_profile"

DEFAULT_PACSTRAP_MIRROR="${APT_PROXY?}"
: ${PACSTRAP_MIRROR:=${DEFAULT_PACSTRAP_MIRROR?}}


DEFAULT_ARCHSTRAP_PKGDIR_SIZE=500m
: ${ARCHSTRAP_PKGDIR_SIZE:=${DEFAULT_ARCHSTRAP_PKGDIR_SIZE}}

PACMAN_CONFFILE_REL="/etc/pacman.conf"
PACMAN_CONFDIR_REL="/etc/pacman.d"
PACMAN_MIRRORLIST_REL="${PACMAN_CONFDIR_REL}/mirrorlist"
MIRRORLIST_REL="${PACMAN_MIRRORLIST_REL}" # FIXME DEPRECATED REMOVE

PACMAN_PKGDIR_REL="/var/cache/pacman/pkg"

ARCHSTRAP_TARGET_OVERLAY_REL="/target-overlay"
ARCHSTRAP_PACKAGE_GRPLIST_REL="/package-group.list.d"


## the following vars are only relevant for bootstrapping the target,
##  except for PACSTRAP_PKG_GRP, ARCHSTRAP_EXPAND_PACKAGE_GRP

## whether the staging dir should be moved to <target>/bootstrap or rm -r'ed
ARCHSTRAP_KEEP_STAGING=n
ARCHSTRAP_TARGET_MEM_TYPE=zram
ARCHSTRAP_TARGET_MEM_SIZE=1000m

## The target install process is split into "phases"
##  This helps to keep the package cache dir size down and
##  may also allow multi-layered setups (e.g. one tmpfs branch per phase)
ARCHSTRAP_TARGET_INSTALL_PHASES="base core extra"

## --noconfirm, --needed get always added
if __verbose__; then
ARCHSTRAP_TARGET_PACINS_QUIET=n
else
ARCHSTRAP_TARGET_PACINS_QUIET=y
fi
ARCHSTRAP_TARGET_PACINS_OPTS=

## ARCHSTRAP_PKG_FILTER applies to both pacstrap/pacinstall
##
##  Note that package filters apply to expanded package lists only
##  (i.e. package groups).
##
ARCHSTRAP_PKG_FILTER=""


## default pacstrap package groups / packages
PACSTRAP_PKG_GRP="base"
PACSTRAP_PKG_EXTRA="openssh nano"
PACSTRAP_PKG_FILTER=""

## list of _additional_ package groups that should be expanded
##   when bootstrapping the staging dir
##
##  (PACSTRAP_PKG_GRP and "base" get always expanded
##   and don't need to be appended to this var)
##
ARCHSTRAP_EXPAND_PACKAGE_GRP=

__HAVE_ARCH_BOOTSTRAP_ENV=y
