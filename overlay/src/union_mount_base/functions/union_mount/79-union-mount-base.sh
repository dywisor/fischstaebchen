## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void _union_mount_done ( fstype, mountpoint )
##
_union_mount_done() {
   union_mount_fstype="${1:?}"
   union_mountpoint="${2:?}"
}

## __union_mount_do_move_container_mount (
##    **old_mp, **union_mount_container, **new_container
## )
##
__union_mount_do_move_container_mount() {
   <%%local rel_mp %>
   <%%local new_mp %>

   rel_mp="${old_mp#${union_mount_container}}/"
   new_mp="${new_container%/}/${rel_mp}"

   xmount_move "${old_mp}" "${new_mp}"
}


## void _union_mount_move_container_to ( new_container_path )
##
_union_mount_move_container_to() {
   <%%local old_mp %>
   <%%local new_container %>
   <%%local aux_mnt_root %>

   union_mount_get_overlayfs_aux_mnt_root

   new_container="${1:?}"
   if [ ! -d "${new_container}" ]; then
      die \
"union_mount_move_container_to(): \
new container does not exist: ${new_container}"
   fi

   case "${new_container}" in
      "${union_mount_container%/}"|\
      "${union_mount_container}/"*)
         die "union_mount_move_container_to: old == new"
      ;;
   esac

   # move old mounts that are expected to be mounted
   for old_mp in \
      "${union_mount_container}/moved/"* \
      "${union_mount_container}/disk/"* \
      "${union_mount_container}/mem/"* \
      "${union_mount_container}/sfs-container" \
      "${union_mount_container}/loop/"* \
      "${union_mount_container}/aux/"* \
      "${aux_mnt_root}/bind/"*
   do
      if [ ! -d "${old_mp}" ]; then
         @@NOP@@

      elif is_mounted "${old_mp}"; then
         __union_mount_do_move_container_mount

      else
         veinfo "${old_mp} should be mounted, but it isn't"
         continue
      fi
   done

   # move old mounts that may or may not be mounted (no veinfo())
   for old_mp in \
      "${aux_mnt_root}/layers/"*
   do
      if [ -d "${old_mp}" ] && is_mounted "${old_mp}"; then
         __union_mount_do_move_container_mount
      fi
   done
}

## void _union_mount_relocate_container_to ( new_container_path )
##
##  Like _union_mount_move_container_to(),
##  but verifies that the old container is "empty" after moving the mounts
##  and unmounts the old container if necessary.
##
_union_mount_relocate_container_to() {
   autodie _union_mount_move_container_to "${1}"

   if is_mounted_verbose "${union_mount_container}[/][^/].*"; then
      die "did not move union base mounts properly"
   fi

   if is_mounted "${union_mount_container}"; then
      autodie umount "${union_mount_container}"
   fi
}

## void union_mount_move_container_to_newroot ( name )
##
union_mount_move_container_to_newroot() {
   <%%local v0 %>
   <%%local new_container %>

   _union_mount_get_container_in_newroot "${1:?}"
   <%%v0 :? new_container %>

   _union_mount_move_container_to "${new_container}"
   union_mount_container="${new_container}"
}
