## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @private void _union_init_common (
##    src_root, mount_root, mount_relpath, name:="", container_path:=""
## )
##
_union_init_common() {
   union_mount_zap_vars

   union_src_root="${1}"
   union_mountpoint="${2%/}/${3#/}"
   union_name="${4:-${3##*/}}"

   if [ -n "${5-}" ]; then
      autodie mkdir -p -- "${5}"
      autodie union_mount_set_container_path "${5}"
   fi
}

## void union_mount_init_initramfs ( mount_relpath, name:="" )
##
union_mount_init_initramfs() {
   veinfo \
      "Initializing new initramfs union mount ${1:-%mp%} (name ${2:-<auto>})"
   _union_init_common / / "${1:?}" "${2-}"
}

## void union_mount_init_newroot ( mount_relpath, name:="" )
##
union_mount_init_newroot() {
   veinfo \
      "Initializing new newroot union mount ${1:-%mp%} (name ${2:-<auto>})"
   _union_init_common "${NEWROOT}" "${NEWROOT}" "${1:?}" "${2-}"
}

## void union_mount_init_newroot_moving ( name:="" )
##
union_mount_init_newroot_moving() {
   veinfo \
"Initializing new initramfs union mount \
that will be moved to newroot later on (name ${1:-union})"

   _union_init_common / /mnt/union union_mnt "${1:-union}" /mnt/union/container

   # moving targets default to import_all=y
   union_mount_import_all=y
}

## void union_mount_init_newroot_itself()
##
union_mount_init_newroot_itself() {
   union_mount_init_newroot_moving rootfs
}
