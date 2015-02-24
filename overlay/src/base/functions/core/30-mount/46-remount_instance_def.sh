## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##




## @autodie void eval_remount_status_functions ( name, mountpoint )
##
##  Creates remount/remount status functions for the given mountpoint:
##
##  void %name_remount_status_setup_flag_files()
##    Must be called before any of the following functions.
##    (Just call it after loading this file)
##
##  void %name_remount_restore_status()
##    Remounts %mountpoint in readonly mode if necessary.
##
##  void %name_no_remount_ro()
##    Declares that %mountpoint's status should not be restored
##    by removing its "remount-ro" flag file.
##    Has to be called _after_ remounting %mountpoint.
##
##  int have_%name_rw()
##    Returns 0 if %mountpoint is writable, else 1.
##
##  int get_%name_rw_nonfatal()
##    Remounts %mountpoint in read-write mode and
##    checks whether it's actually writable.
##    Returns 0 if %mountpoint is writable, else 1.
##
##  void get_%name_rw()
##    Remounts %mountpoint in read-write mode and
##    checks whether it's actually writable.
##
##
##  %name and %mountpoint may also be references (e.g. \$NEWROOT)
##
eval_remount_status_functions() {
   <%%varcheck 1..2 %>
   eval "
${1}_remount_setup_flag_files() {
   _remount_status_setup_flag_files \"${1}\" \"${2}\"
}

${1}_remount_restore_status() {
   _remount_status_restore_mount \"${1}\" \"${2}\"
}

${1}_no_remount_ro() {
   _remount_status_no_remount_ro \"${1}\" \"${2}\"
}

have_${1}_rw() {
   _remount_status_check_have_rw \"${1}\" \"${2}\"
}

get_${1}_rw_nonfatal() {
   _remount_status_get_rw_check \"${1}\" \"${2}\"
}

get_${1}_rw() {
   autodie _remount_status_get_rw_check \"${1}\" \"${2}\"
}
" || die "failed to eval remount functions: ${*}"
}
