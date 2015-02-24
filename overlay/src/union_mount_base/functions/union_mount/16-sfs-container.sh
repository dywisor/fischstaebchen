## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void union_mount_get_sfs_container()
##
union_mount_get_sfs_container() {
   if [ -z "${union_sfs_container}" ]; then

      if is_mounted "${_union_sfs_container_path}"; then
         xremount_rw "${_union_sfs_container_path}"
      else
         autodie dyncontainer_init_globals
         autodie _dyncontainer_init \
            "${_union_sfs_container_path}" sfs-container
      fi

      union_sfs_container="${_union_sfs_container_path}"
   fi
}

## void union_mount_finalize_sfs_container()
##
union_mount_finalize_sfs_container() {
   [ -n "${union_sfs_container}" ] || return 0

   autodie _dyncontainer_downsize "${union_sfs_container}"
   union_sfs_container=
}

_union_mount_intercept_file_import() {
   <%%local v0 %>

   case "${type}" in
      local)
         _prefix_path_with_union_src_root "${uri}"
         <%%v0 _ uri %>
         _union_last_import_filepath="${uri}" # might be (re-)set later on

         if [ "${union_mount_import_all:-n}" != "y" ]; then
            dst="${uri}"
            return 1
         fi
      ;;

      disk|aux)
         true
      ;;

      *)
         intercept_file_import_do_check_have_net || return ${?}
      ;;
   esac

   # lazy-init sfs container
   union_mount_get_sfs_container

   einfo "Importing sfs file ${uri} => ${dst##*/} (type=${type})"
   return 0
}

_union_mount_file_import_done() {
   _union_last_import_filepath="${src_filepath-}"
}

## int _union_mount_container_import ( src_uri, name )
_union_mount_container_import() {
   <%%locals -p F_IMPORT_FILE_ INTERCEPT DONE %>

   if [ -z "${_union_sfs_container_path-}" ]; then
      die "container path needs to be set before importing sfs files!"
   fi

   F_IMPORT_FILE_INTERCEPT=_union_mount_intercept_file_import
   F_IMPORT_FILE_DONE=_union_mount_file_import_done

   _union_last_import_filepath=

   import_file "${_union_sfs_container_path}" "${1:?}" "${2:?}"
}
