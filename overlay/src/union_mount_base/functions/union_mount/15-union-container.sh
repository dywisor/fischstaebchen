## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @private void _union_mount_get_container_in_newroot ( [name], **v0! )
##
##  Creates a mount container in %NEWROOT for the currently configured union
##  mount. Stores the mount container's path in %v0.
##
_union_mount_get_container_in_newroot() {
   <%%retvar v0 %>
   <%%locals name %>
   <%%locals -p container_ relpath path %>

   [ -z "${1+SET}" ] || { name="${1}"; shift; }
   <%%varcheck name:=${union_name:?} %>

   container_relpath="/.${name}_union_root"
   container_path="${NEWROOT%/}/${container_relpath#/}"

   if \
      test_fs_is_real_dir "${container_path}" || \
      { get_newroot_rw_nonfatal && mkdir -p -- "${container_path}"; }
   then
      xmount "${container_path}" container tmpfs "${MNT_CONTAINER_OPTS}"

   else
      container_path="${NEWROOT}/mnt/${container_relpath#/}"
      xmount_mnt_container "${NEWROOT}/mnt" # no-op if already mounted
   fi

   #container_relpath= ## undefined value

   veinfo "Creating union container: ${container_path}"
   autodie mkdir -p -- "${container_path}"
   v0="${container_path}"
}

## void union_mount_get_container_in_newroot ( [name] )
##
##  Creates mount container in %NEWROOT and sets the union mount container path.
##
union_mount_get_container_in_newroot() {
   <%%local v0 %>
   autodie _union_mount_get_container_in_newroot "$@"
   union_mount_set_container_path "${v0:?}"
}
