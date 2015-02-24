## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int union_mount_find_file__rotate_old ( oldfile_path )
##
union_mount_find_file__rotate_old() {
   <%%local datestr %>

   ## Yes, this always executes two date commands,
   ## even if the second one is not needed
   ##
   ##  Too lazy to fix this.
   ##
   for datestr in "$(date +%F)" "$(date +%s)"; do
      if [ -z "${datestr}" ]; then
         eerror "Cannot rotate sfs files: failed to get date str"
         return 1

      elif [ ! -f "${1}.${datestr}" ]; then
         einfo "Rotating sfs file: ${1##*/} => ${1##*/}.${datestr}"
         autodie mv -- "${1}" "${1}.${datestr}"
         return ${?}

      fi
   done

   eerror "Cannot rotate sfs files: could not move ${1##*/}"
   return 1
}

## int union_mount_find_file__rotate_base_to_old ( basepath )
##
union_mount_find_file__rotate_base_to_old() {
   if [ -f "${1:?}.old" ]; then
      union_mount_find_file__rotate_old "${1:?}.old" || return ${?}
   fi

   einfo "Rotating sfs file: ${1##*/} => ${1##*/}.old"
   autodie mv -- "${1}" "${1}.old"
}

## int union_mount_find_file__rotate_new ( sfs_filepath, **sfs_filepath! )
##
union_mount_find_file__rotate_new() {
   if [ -f "${basepath}" ]; then
      union_mount_find_file__rotate_base_to_old "${basepath}" || return 0
   fi

   einfo "Rotating sfs file: ${1##*/} => ${basepath##*/}"

   if ln -- "${1}" "${basepath}"; then
      sfs_filepath="${basepath}"

      # could be made non-fatal:
      autodie rm -- "${1}"
   else
      ewarn "Could not hardlink ${basepath}, falling back to mv"

      autodie mv -- "${1}" "${basepath}"
      sfs_filepath="${basepath}"
   fi

   return 0
}

## int union_mount_find_file__do_rotate ( sfs_filepath, **... )
##
union_mount_find_file__do_rotate() {
   <%%local basepath %>

   case "${oldnew_match_suffix#.}" in
      new)
         basepath="${1%.new}"
         autodie test "${1}" != "${basepath}"

         case "${1}" in
            "${NEWROOT}/"*)
               ## FIXME: stat $NEWROOT, $1, check same fs
               autodie get_newroot_rw
            ;;
         esac

         autodie union_mount_find_file__rotate_new "${1}"
      ;;
   esac
}
