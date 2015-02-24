## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @private void _union_do_mount_sfs_file ( sfs_file, mountpoint_name ), raises die()
##
_union_do_mount_sfs_file() {
   <%%local mp=${union_mount_container}/loop/${2:?} %>

   veinfo "Mounting sfs file ${1} as ${2}"

   xmount "${mp}" "${1:?}" squashfs ro,loop
   _union_mount_add_branch rr "${mp}"
}

## void union_mount_sfs_files(), raises die()
##
union_mount_sfs_files() {
   <%%locals overlay_file -i k %>

   union_mount_finalize_sfs_container

   veinfo "Mounting sfs files"

   if [ -n "${union_sfs_file}" ]; then
      _union_do_mount_sfs_file "${union_sfs_file}" base
   fi

   k=0
   for overlay_file in ${union_sfs_overlay_files}; do
      _union_do_mount_sfs_file "${overlay_file}" "overlay_${k}"
      <%%inc k %>
   done
}
