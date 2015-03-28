#!@@XSHELL@@
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -u

! ishare_has_flag no-makedirs || exit 0

_newroot_dodir() {
   veinfo "Creating directory ${1} in newroot"

   mkdir -p -- "${NEWROOT}/${1#/}" || \
      ewarn "Failed to create directory ${1} in newroot"
}

_filter_fstab_entry_has_mountpoint() {
   [ -z "${BOGUS-}" ] || return 1

   case "${fs-}" in
      ''|'#'*)
         return 1
      ;;
   esac

   case "${fstype-}" in
      ''|ignore|swap)
         return 1
      ;;
   esac

   case "${mp-}" in
      /*)
         return 0
      ;;
   esac

   return 1
}


run_make_mountdirs() {
   <%%locals fs mp fstype opts dump pass BOGUS %>

   [ -f "${1}" ] || return 0

   while read -r fs mp fstype opts dump pass BOGUS; do

      if _filter_fstab_entry_has_mountpoint; then
         _newroot_dodir "${mp}"
      fi

   done < "${1}"
}

run_makedirs() {
   <%%local line %>

   [ -f "${1}" ] || return 0

   while read -r line; do
      case "${line}" in
         ''|'#'*)
            true
         ;;
         *)
            _newroot_dodir "${line}"
         ;;
      esac
   done < "${1}"
}


makedirs_main() {
   <%%local rc=0 %>

   run_makedirs         "${makedirs_file}"   || rc=${?}
   run_make_mountdirs   "${fstab_file}"      || rc=${?}

   return ${rc}
}

fstab_file="${NEWROOT}/etc/fstab"
makedirs_file="${NEWROOT_CONFDIR}/makedirs"

if [ -f "${fstab_file}" ] || [ -f "${makedirs_file}" ]; then
   # shellcheck disable=SC2015
   get_newroot_rw_nonfatal && makedirs_main || \
      ewarn "Failed to create directories in newroot!"
fi
