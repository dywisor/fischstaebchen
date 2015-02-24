#!/bin/sh
##  Do not source this file directly, load either
##
##  * archstrap-target  for setting up a target (w/ intermediate staging) OR
##  * archstrap-staging for setting up staging only
##
##
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if [ -n "${__HAVE_ARCH_BOOTSTRAP_BASE_FUNCTIONS-}" ]; then
   die "arch bootstrap base functions already loaded!"
fi

__HAVE_ARCH_BOOTSTRAP_BASE_FUNCTIONS=y

if [ -z "${__HAVE_ARCH_BOOTSTRAP_ENV-}" ]; then
   loadscript_or_die "${__DIR__}/env.sh"
fi
: ${_ARCH_BOOTSTRAP_SRCDIR:?}


_archstrap_append_to_pkg_filter() {
   ARCHSTRAP_PKG_FILTER="${ARCHSTRAP_PKG_FILTER} ${*}"
}


_archstrap_zap_fspaths() {
   WORKDIR=
   pacman_conffile=
   pacman_confdir=
   pacman_mirrorlist=
   pacman_pkgdir=
}

_archstrap_set_fspaths() {
   local r
   r="${1%/}"

   pacman_conffile="${r}/${PACMAN_CONFFILE_REL#/}"
   pacman_confdir="${r}/${PACMAN_CONFDIR_REL#/}"
   pacman_mirrorlist="${r}/${PACMAN_MIRRORLIST_REL#/}"
   pacman_pkgdir="${r}/${PACMAN_PKGDIR_REL#/}"
}

__archstrap_set_workdir() {
   _archstrap_zap_fspaths

   case "${1-}" in
      /*)
         WORKDIR="${1}"
         _archstrap_set_fspaths "${WORKDIR}"
      ;;
      *)
         die "bad archstrap workdir: ${1:-<empty>}"
      ;;
   esac
}

__archstrap_mount_pkgdir() {
   <%%varcheck 1 %>

   if is_mounted "${1%/}/${PACMAN_PKGDIR_REL#/}"; then
      ewarn "pkgdir already mounted in ${1}"
   else
      xmount "${1%/}/${PACMAN_PKGDIR_REL#/}" pacpkg tmpfs \
         "mode=0755,size=${ARCHSTRAP_PKGDIR_SIZE:-${DEFAULT_ARCHSTRAP_PKGDIR_SIZE:?}}"
   fi
}

__archstrap_umount_pkgdir() {
   <%%varcheck 1 %>

   if is_mounted "${1%/}/${PACMAN_PKGDIR_REL#/}"; then
      autodie umount "${1%/}/${PACMAN_PKGDIR_REL#/}"
   else
      ewarn "pkgdir not mounted in ${1}"
   fi
}

__archstrap_basemounts() {
   <%%varcheck 1 %>

   __archstrap_mount_pkgdir "${1}"
   autodie stagedive_bootstrap_basemounts "${1}"
}

__archstrap_eject_basemounts() {
   <%%varcheck 1 %>

   sync
   stagedive_bootstrap_kill_chroot_processes "${1}"
   sync

   if is_mounted "${1%/}/mnt/target"; then
      autodie umount "${1%/}/mnt/target"
   fi

   __archstrap_umount_pkgdir "${1}"
   autodie stagedive_bootstrap_eject_basemounts "${1}"
}

__archstrap_chroot() {
   (
      exec 0</dev/null || exit
      mkdir -p -m 0700 -- "${1:?}/root" || exit

      if __quiet__; then
         exec 1>/dev/null 2>/dev/null || exit
      fi

      export HOME=/root
      exec chroot "${@}"
   ) && fail=0 || fail=${?}

   return ${fail}
}

## @autodie movedir_into ( srcdir, dstdir )
##
##  Moves all (/most) files from %srcdir to %dstdir.
##  Fails if any file already exists in %dstdir,
##  so %dstdir should be almost empty when calling this function.
##
movedir_into() {
   <%%varcheck 1..2 %>
   <%%locals src name dst %>

   for src in "${1%/}/"* "${1%/}/."*; do
      if _test_fs_lexists "${src}"; then
         name="${src##*/}"

         case "${name}" in
            '.'|'..')
               @@NOP@@
            ;;

            'lost+found')
               ## ext[234]
               rmdir -- "${src}" || ewarn "Failed to remove ${src}"
            ;;

            *)
               dst="${2%/}/${name}"
               if _test_fs_lexists "${dst}"; then
                  die "Cannot move ${name} to ${2%/}/: file exists"
               else
                  autodie mv -- "${src}" "${dst}"
               fi
            ;;
         esac
      fi
   done
}

__archstrap_process_pkglist_file_line() {
   case "${1}" in
      '') ewarn "empty line???" ;;

      G|GRP|@)
         shift || return
         [ ${#} -eq 0 ] || grp_list="${grp_list:+${grp_list} }${*}"
      ;;
      GE|@@)
         shift || return
         [ ${#} -eq 0 ] || \
            grp_expandonly_list="${grp_expandonly_list:+${grp_expandonly_list} }${*}"
      ;;
      P|PKG|pkg)
         shift || return
         [ ${#} -eq 0 ] || pkg_list="${pkg_list:+${pkg_list} }${*}"
      ;;
      X|AUR|aur)
         shift || return
         ewarn "${infile:-???}: ${lino:-???}: AUR line ignored."
      ;;
      F|FILTER|filter)
         shift || return
         ewarn "${infile:-???}: ${lino:-???}: filter line ignored."
      ;;
      @@*)
         grp_expandonly_list="${grp_expandonly_list:+${grp_expandonly_list} }${*#@@}"
      ;;
      @*)
         grp_list="${grp_list:+${grp_list} }${*#@}"
      ;;
      *)
         pkg_list="${pkg_list:+${pkg_list} }${*}"
      ;;
   esac
}

__archstrap_read_pkglist_file() {
   einfo "Reading pkglist file ${1##*/}"
   if read_text_file_do "${1}" __archstrap_process_pkglist_file_line; then
      veinfo "read pkglist ${1}: success"
      return 0
   else
      eerror "Failed to read pkglist file ${1} (rc=${?})"
      return 1
   fi
}


archstrap_read_pkglist_files() {
   <%%retvars pkg_list grp_list grp_expandonly_list %>

   while [ ${#} -gt 0 ]; do
      if [ -f "${1}" ]; then
         __archstrap_read_pkglist_file "${1}" || return ${?}
      else
         eerror "Cannot find pkglist file: '${1}'"
         return 2
      fi
      shift
   done
}

archstrap_read_pkglist_files_if_exist() {
   <%%retvars pkg_list grp_list grp_expandonly_list %>

   while [ ${#} -gt 0 ]; do
      if [ -f "${1}" ]; then
         __archstrap_read_pkglist_file "${1}" || return ${?}
      fi
      shift
   done
}
