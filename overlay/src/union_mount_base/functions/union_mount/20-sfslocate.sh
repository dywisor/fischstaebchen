## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int union_mount_set_file ( file_relpath )
##
##  Adds a base squashfs file to the union mount.
##  Its path is interpreted relative to the union src root.
##
union_mount_set_file() {
   <%%local v0 %>

   [ -n "${1-}" ] || return 1

   _union_mount_container_import "${1}" base.sfs || return 1

   union_sfs_file="${v0}"

   union_sfs_file_srcpath="${_union_last_import_filepath}"
   _union_last_import_filepath=

   [ -n "${union_sfs_file}" ] || return 1
   [ -f "${union_sfs_file}" ] || return 1

   veinfo "Using sfs file ${union_sfs_file}"
   return 0
}

## int union_mount_find_file ( *basepaths )
##
##  Searches for any sfs file that matches one of the given basepaths:
##   filepath := %union_src_root/%basepath.sfs{.new,,.old}.
##
##  Returns 0 on success, else 1.
##
union_mount_find_file() {
   <%%locals v0 v1 %>
   <%%locals -p oldnew_match_ base suffix %>
   <%%locals sfs_filepath %>

   get_any_file_oldnew_from "${union_src_root}" "$@" || return 1
   <%%v1 :? sfs_filepath %>

   if [ "${union_mount_rotate_files:?}" = "y" ]; then
      autodie union_mount_find_file__do_rotate "${sfs_filepath}"
   fi

   union_mount_set_file "${sfs_filepath:?}"
}

## int union_mount_set_or_find_file ( file_relpath, *basepaths )
##
##  Calls union_mount_set_file(%file_relpath) if %file_relpath is not empty,
##  else union_mount_find_file(*basepaths).
##
union_mount_set_or_find_file() {
   case "${1-}" in
      ''|'none')
         return 0
      ;;

      '--')
         @@SHIFT_OR_RET@@
      ;;

      'auto')
         @@SHIFT_OR_RET@@
         [ "${1:-X}" != "--" ] || @@SHIFT_OR_RET@@
      ;;

      *)
         union_mount_set_file "${1}" || return ${?}
         return 0
      ;;
   esac

   union_mount_find_file "$@"
}

## int _union_mount_do_add_overlay_file ( uri )
##
_union_mount_do_add_overlay_file() {
   <%%local v0 %>
   <%%local -i idx=${union_sfs_overlay_file_index_next:?} %>

   if \
      { ! _union_mount_container_import "${1}" "overlay_${idx}.sfs"; } || \
      [ -z "${v0}" ]
   then
      eerror "failed to import overlay file: ${1}"
      return 1

   elif [ ! -f "${v0}" ]; then
      eerror "no such overlay file: ${v0}"
      return 1
   fi

   veinfo "Adding sfs overlay file ${v0}"
   _union_sfs_overlay_files_append "${v0}"

   # also inc on error?
   <%%inc union_sfs_overlay_file_index_next %>

   return 0
}



## int union_mount_add_overlay_files ( *file_relpaths )
##
##  Locates and appends zero or more files to the list of sfs overlay files.
##
##  Immediately returns non-zero if any of the given files could not be found.
##  !!! This behavior might change in future
##       (i.e. keep going and return error after processing all files)
##
union_mount_add_overlay_files() {
   while [ $# -gt 0 ]; do
      [ -z "${1}" ] || _union_mount_do_add_overlay_file "${1}" || return ${?}

      shift
   done
}

## int _union_mount_vcheck_can_expand_incremental()
##
if __verbose__; then
_union_mount_vcheck_can_expand_incremental() {
   <%%local msg="Cannot expand incremental overlay files" %>

   if [ -z "${union_sfs_file}" ]; then
      eerror "${msg} - base file is not set!"
      return 1

   elif [ -z "${union_sfs_file_srcpath}" ]; then
      eerror "${msg} for ${union_sfs_file##*/} - srcpath is not set!"
      eerror "Did you import the base file via http[s]/ftp?"
      return 2

   elif [ ! -f "${union_sfs_file_srcpath}" ]; then
      eerror "${msg} for ${union_sfs_file##*/} - srcpath does not exist"
      eerror "(${union_sfs_file_srcpath} is not a file)"
      return 3
   fi

   return 0
}
else
_union_mount_vcheck_can_expand_incremental() {
   [ -n "${union_sfs_file}" ]          || return 1
   [ -n "${union_sfs_file_srcpath}" ]  || return 2
   [ -f "${union_sfs_file_srcpath}" ]  || return 3
}
fi

## void union_mount_expand_incremental()
##
##  Appends all files matching
##    <union sfs file path w/o suffix>_<incremental suffix>[.<filename suffix]
##  to the list of sfs overlay files.
##
##  Note: should be called before add_overlay_files().
##
union_mount_expand_incremental() {
   <%%locals basename basepath suffix fabs %>

   _union_mount_vcheck_can_expand_incremental || \
      die "Cannot expand incremental overlay files!"

   basename="${union_sfs_file_srcpath##*/}"

   case "${basename}" in
      *.*)
         basepath="${union_sfs_file_srcpath%.*}"
         suffix=".${basename##*.}"
      ;;
      *)
         basepath="${union_sfs_file_srcpath}"
         suffix=
      ;;
   esac

   for fabs in "${basepath}_"*"${suffix}"; do
      if [ -f "${fabs}" ]; then
         # double-slash enforces "true" absolute path
         autodie _union_mount_do_add_overlay_file "file=/${fabs}"
      fi
   done
}
