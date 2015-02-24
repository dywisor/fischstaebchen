## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<%weakdef _AUTODIE_OPTIONAL autodie %>
## TODO:
##  add a "force-no-remount" flag
##  * the current solution is to wipe the "need-remount-ro" flag after
##    (possibly) remounting %mountpoint
##  * should "force-no-remount" be preserved between hook phases?
##

## _remount_status_setup_flag_files ( flag_name, mountpoint )
_remount_status_setup_flag_files() {
   <%%varcheck 1..2 %>

   # functions should set (or remove) "need-remount-ro-%flag_name"
   @@_AUTODIE_OPTIONAL@@ ishare_del_flag "need-remount-ro-${1}" || return

   # hooks should check for "have-%flag_name-rw"
   if check_dir_writable "${2}"; then
      veinfo "initial mount status of ${2}: read-write"
      @@_AUTODIE_OPTIONAL@@ ishare_add_flag "have-mount-rw-${1}"
   else
      veinfo "initial mount status of ${2}: readonly"
      @@_AUTODIE_OPTIONAL@@ ishare_del_flag "have-mount-rw-${1}"
   fi
}

## void _remount_status_restore_mount ( flag_name, mountpoint )
_remount_status_restore_mount() {
   <%%varcheck 1..2 %>

   if ishare_has_flag "need-remount-ro-${1}"; then
      einfo "Restoring readonly mount ${2}"
      xremount_ro "${2}"
      @@_AUTODIE_OPTIONAL@@ ishare_del_flag "need-remount-ro-${1}"
      @@_AUTODIE_OPTIONAL@@ ishare_del_flag "have-mount-rw-${1}"
   fi
}

## void _remount_status_no_remount_ro ( flag_name, [<mountpoint ignored>] )
_remount_status_no_remount_ro() {
   @@_AUTODIE_OPTIONAL@@ ishare_del_flag "need-remount-ro-${1}"
}

## int _remount_status_check_have_rw ( flag_name, [<mountpoint ignored>] )
_remount_status_check_have_rw() {
   ishare_has_flag "have-mount-rw-${1:?}"
}

## int _remount_status_get_rw ( flag_name, mountpoint )
_remount_status_get_rw() {
   <%%varcheck 1..2 %>

   if _remount_status_check_have_rw "${@}"; then
      veinfo "${2} is already mounted in read-write mode."
      return 0
   fi

   veinfo "Remounting ${2} in read-write mode"
   @@_AUTODIE_OPTIONAL@@ ishare_add_flag "need-remount-ro-${1}" || return
   xremount_rw "${2}" || return
   @@_AUTODIE_OPTIONAL@@ ishare_add_flag "have-mount-rw-${1}"
}

## int _remount_status_get_rw_check ( flag_name, mountpoint )
##
_remount_status_get_rw_check() {
   _remount_status_get_rw "${@}" || return

   if check_dir_writable "${2}"; then
      @@_AUTODIE_OPTIONAL@@ ishare_add_flag "have-mount-rw-${1}"
      return 0
   else
      eerror "Remounted ${2} in read-write mode, but it's still readonly!"
      @@_AUTODIE_OPTIONAL@@ ishare_del_flag "have-mount-rw-${1}"
      return 1
   fi
}
