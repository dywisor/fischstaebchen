## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<%weakdef UNION_MOUNT_KNOWN_FS_TYPES aufs overlayfs %>

_union_detect_default_fstype() {
   <%%locals fstype %>

   veinfo "Trying to find a supported union fstype"

   for fstype in ${1:-@@UNION_MOUNT_KNOWN_FS_TYPES@@}; do
      if check_fstype_supported "${fstype}"; then
         veinfo "Using ${fstype} as union fstype"

         _UNION_FS_TYPE="${fstype}"
         add_env _UNION_FS_TYPE "${fstype}"
         return 0
      fi
   done

   ewarn "Could not find any union fstype!"
   return 1
}

_union_get_default_fstype() {
   [ -n "${_UNION_FS_TYPE-}" ] || \
      _union_detect_default_fstype ${UNION_MOUNT_FSTYPE-}
}

# shellcheck disable=SC2120
union_mount_set_fstype() {
   if [ -z "${1-}" ]; then
      _union_get_default_fstype

      [ -n "${_UNION_FS_TYPE-}" ] || \
         die "no overlay filesystem type found!"

   else
      if [ "${1}" != "${_UNION_FS_TYPE-}" ]; then
         _UNION_FS_TYPE="${1}"
         add_env _UNION_FS_TYPE "${_UNION_FS_TYPE}"
      fi

      ewarn "Using unchecked union fstype ${_UNION_FS_TYPE}"
   fi

   return 0
}
