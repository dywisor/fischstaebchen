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

## initially, there's no target and therefore no target basemounts
TARGET_HAVE_BASEMOUNTS=

archstrap_set_target() {
   __archstrap_set_workdir "${@}"; TARGET="${WORKDIR}"
}

archstrap_target_zap_pkgdir() {
   __archstrap_umount_pkgdir "${TARGET:?}" && \
   __archstrap_mount_pkgdir  "${TARGET:?}"
}

archstrap_target_basemounts() {
   : ${TARGET:?}
   if [ "${TARGET_HAVE_BASEMOUNTS:-X}" = "y" ]; then
      veinfo "target basemounts already mounted."
   else
      __archstrap_basemounts "${TARGET}" && TARGET_HAVE_BASEMOUNTS=y
   fi
}

archstrap_target_eject_basemounts() {
   __archstrap_eject_basemounts "${TARGET:?}" && TARGET_HAVE_BASEMOUNTS=
}

archstrap_target_eject_basemounts_if_mounted() {
   [ "${TARGET_HAVE_BASEMOUNTS:-X}" != "y" ] || \
   archstrap_target_eject_basemounts
}

_archstrap_chroot_target() {
   __archstrap_chroot "${TARGET:?}" "${@}"
}

archstrap_chroot_target() {
   archstrap_target_basemounts && \
   __archstrap_chroot "${TARGET:?}" "${@}"
}

add_extra_pacstrap_pkg() {
   PACSTRAP_PKG_EXTRA="${PACSTRAP_PKG_EXTRA?} ${*}"
}

read_extra_pacstrap_pkg_from() {
   [ -f "${1:?}" ] && read_text_file_do "${1}" add_extra_pacstrap_pkg
}

# int get_pacstrap_pkg_list ( **pacstrap_pkg_list! )
get_pacstrap_pkg_list() {
   pacstrap_pkg_list=
   local pkg_install_list

   autodie _archstrap_get_package_install_list \
      "${PACSTRAP_PKG_FILTER?}" \
      "${PACSTRAP_PKG_GRP?}" "${PACSTRAP_PKG_EXTRA?}"

   pacstrap_pkg_list="${pkg_install_list}"
   [ -n "${pacstrap_pkg_list}" ]
}

## int _archstrap_get_package_install_list (
##    filter, grp_list, *pkg_list, **pkg_install_list!
## )
##
_archstrap_get_package_install_list() {
   pkg_install_list=
   [ ${#} -ge 2 ] || return @@EX_USAGE@@
   local v0

   if [ -n "${2}" ]; then
      archstrap_get_pkg_grp_list "${1?}" "${2?}" || return
      pkg_install_list="${v0}"
   fi

   shift 2 || return
   # shellcheck disable=SC2048
   set -- ${*}
   [ "${1:-X}" != "--" ] || shift

   [ ${#} -eq 0 ] || \
      pkg_install_list="${pkg_install_list:+${pkg_install_list} }${*}"
}

## int archstrap_get_pkg_grp_list ( "-"|filter_append, *grp_name, **v0! )
archstrap_get_pkg_grp_list() {
   <%%retvar v0 %>
   [ ${#} -gt 0 ] || return @@EX_USAGE@@
   [ ${#} -gt 1 ] || return 0

   local arg
   arg="${1?}"; shift
   case "${arg:-X}" in
      --|-) arg=
   esac

   ## also filter all listed groups + expandonly groups
arg="${ARCHSTRAP_PKG_FILTER?} ${arg} ${*} ${PACSTRAP_PKG_GRP?} ${ARCHSTRAP_EXPAND_PACKAGE_GRP?}"

   _archstrap_get_pkg_grp_list "${arg}" "${@}"
}

## int _archstrap_get_pkg_grp_list ( filter, *grp_name, **v0! )
_archstrap_get_pkg_grp_list() {
   <%%retvar v0 %>
   [ ${#} -gt 0 ] || return @@EX_USAGE@@
   <%%locals expr pkg_grp_list pkg_grp_file filter_arg %>

   filter_arg="${1?}"; shift

   # shellcheck disable=SC2048
   set -- ${*}
   [ ${#} -gt 0 ] || return @@EX_USAGE@@

   autodie get_regex_or_expr_exact "${filter_arg}"
   [ -n "${expr}" ] || die "%expr list empty, but grp list is not."

   while [ ${#} -gt 0 ]; do
      pkg_grp_file="${ARCHSTRAP_PKG_GRPLIST_DIR:?}/${1}"
      pkg_grp_list="$( grep -Ev -- "${expr}" "${pkg_grp_file}" )"

      if [ -n "${pkg_grp_list}" ]; then
         v0="${v0:+${v0} }${pkg_grp_list}"

      elif [ ! -f "${pkg_grp_file}" ]; then
         eerror "Package group file does not exist: ${pkg_grp_file}"
         return 2

      elif [ -s "${pkg_grp_file}" ]; then
         ewarn "Package group filtered out: ${1}"

      else
         eerror "Package group is empty: ${1}"
         return 4
      fi

      shift
   done
}
