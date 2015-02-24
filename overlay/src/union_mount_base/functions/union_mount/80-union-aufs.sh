## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

UNION_MOUNT_AUFS_OPTIONS="dirperm1"

union_mount_aufs() {
   <%%locals mp opts %>

   mp="${1:-${union_mountpoint:?}}"

   veinfo "Mounting aufs union ${union_name:-%name%} on ${mp}"

   if [ -z "${union_mount_branches}" ]; then
      die "no branches configured!"
   fi

   opts="br:${union_mount_branches}"
   if [ -n "${UNION_MOUNT_AUFS_OPTIONS-}" ]; then
      opts="${UNION_MOUNT_AUFS_OPTIONS},${opts}"
   fi

   xmount "${mp}" "${union_name:-undef}_union" aufs  "${opts}"

   _union_mount_done aufs "${mp}"
}
