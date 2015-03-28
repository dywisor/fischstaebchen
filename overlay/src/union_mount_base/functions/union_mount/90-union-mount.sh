## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

union_mount_it() {
   # shellcheck disable=SC2119
   union_mount_set_fstype

   case "${_UNION_FS_TYPE}" in
      overlay)
         union_mount_overlayfs "${@}"
      ;;
      *)
         union_mount_${_UNION_FS_TYPE} "${@}"
      ;;
   esac
}
