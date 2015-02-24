## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void union_mount_add_move_mount ( old_mountpoint, mount_name, mode=<auto> )
##
union_mount_add_move_mount() {
   <%%local d=${union_mount_container%/}/moved/${2:?} %>

   if [ "$(stat -c '%m' "${d}" @@NO_STDERR@@)" = "${d}" ]; then
<% if FEATURE_PHASE_RETRY %>
      die "${d} is already mounted and phase-retry logic is missing."
<% else %>
      die "${d} is already mounted."
<% endif %>
   fi

   einfo "Moving mount: ${1} => ${d}"
   xmount_move "${1}" "${d}"

   case "${3-}" in
      ro|rw)
         xremount_${3} "${d}"
         _union_mount_add_branch "${3}" "${d}"
      ;;
      *)
         if __quietly__ touch "${d}/.__union_check_writable"; then
            rm -f -- "${d}/.__union_check_writable"
            _union_mount_add_branch rw "${d}"
         else
            _union_mount_add_branch ro "${d}"
         fi
      ;;
   esac
}
