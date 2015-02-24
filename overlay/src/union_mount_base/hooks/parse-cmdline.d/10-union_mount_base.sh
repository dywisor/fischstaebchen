## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

parse_union_mount_base() {
   case "${key}" in
      union_fstype|union_mount_fstype)
         UNION_MOUNT_FSTYPE="${value:-}"
      ;;

      aufs)
         UNION_MOUNT_FSTYPE="${key}"
      ;;

      overlayfs)
         UNION_MOUNT_FSTYPE=overlay
      ;;

      *)
         return 1
      ;;
   esac

   return 0
}

parse_union_mount_base_done() {
   : ${UNION_MOUNT_FSTYPE=}

   add_env_var  \
      UNION_MOUNT_FSTYPE

   return 0
}

add_parser union_mount_base
