#!/bin/sh
##  Do not source this file directly.
## _functions.sh and env.sh must be loaded prior to this file.
##
##
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

: ${__HAVE_ARCH_BOOTSTRAP_BASE_FUNCTIONS:?}

_archstrap_do_run_bootstrap_staging() {
   set --
   <%% foreach_static var !\
      | S B D FORCE_UNPACK KEEPMNT !\
         | ::: {I}[ -n "${{{var}-}}" ] || set -- "${{@}}" {var} !\
   %>

   [ ${#} -eq 0 ] || die "Cannot bootstrap staging: var(s) not set: ${*}"
   runscript_simple_or_die "${_ARCH_BOOTSTRAP_SRCDIR:?}/do_bootstrap_staging.sh"
}

archstrap_set_staging() {
   __archstrap_set_workdir "${@}"; STAGING="${WORKDIR}"
}

archstrap_set_extra_staging_paths() {
   <%%local d=${1:-${STAGING:?}} %>
   ARCHSTRAP_TARGET_OVERLAY="${d%/}/${ARCHSTRAP_TARGET_OVERLAY_REL#/}"
   ARCHSTRAP_PKG_GRPLIST_DIR="${d%/}/${ARCHSTRAP_PACKAGE_GRPLIST_REL#/}"
}

archstrap_staging_basemounts() {
   __archstrap_basemounts "${STAGING:?}"
}

archstrap_staging_eject_basemounts() {
   __archstrap_eject_basemounts "${STAGING:?}"
}

archstrap_chroot_staging() { __archstrap_chroot "${STAGING:?}" "${@}"; }


## @autodie copyfile_to_target_overlay ( srcfile_abs, dstpath_rel )
##
copyfile_to_target_overlay() {
   : ${1:?} ${2:?} ${ARCHSTRAP_TARGET_OVERLAY:?}
   <%%local dstfile=${ARCHSTRAP_TARGET_OVERLAY%/}/${2#/} %>

   dodir "${dstfile%/*}"
   autodie copyfile "${1:?}" "${dstfile}"
}

## @autodie copyfile_workdir_to_target_overlay ( srcfile_rel, [dstpath_rel] )
##
copyfile_workdir_to_target_overlay() {
   : ${WORKDIR:?} ${1:?}
   copyfile_to_target_overlay "${WORKDIR%/}/${1#/}" "${2:-${1}}"
}

## autodie copyfile_staging_to_target_overlay ( srcfile_rel, [dstpath_rel] )
##
copyfile_staging_to_target_overlay() {
   : ${STAGING:?} ${1:?}
   copyfile_to_target_overlay "${STAGING%/}/${1#/}" "${2:-${1}}"
}
