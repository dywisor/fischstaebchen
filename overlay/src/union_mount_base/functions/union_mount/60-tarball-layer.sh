## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void union_mount_add_tarball_layer (
##    name, mem_type:="zram", size:="50%", *file_uri
## )
##
union_mount_add_tarball_layer() {
   if [ ${#} -lt 3 ]; then
      return @@EX_USAGE@@
   elif [ ${#} -eq 3 ]; then
      veinfo "${union_name:-%union_name%}: No tarballs configured (${1})."
      return 0
   fi

   <%%locals mp %>

   union_mount_add_backing_mem_branch \
      ro "tarball-${1:?}" "tb" "${2:--}" "${3:--}"
   shift 3 || die

   union_mount_get_and_unpack_tarball "${mp:?}" "${@}"
}

## @autodie _union_mount_get_and_unpack_tarball ( *file_uri, **dst_dir )
##
_union_mount_get_and_unpack_tarball() {
   <%%varcheck dst_dir %>
   <%%locals tarball_file fail %>

   while [ ${#} -gt 0 ]; do
      autodie union_mount_get_tarball "${1}"

      einfo "Unpacking ${tarball_file}"
      autodie test -f "${tarball_file}"
      @@PROG_UNTAR@@ -C "${dst_dir}/" "${tarball_file}" && fail=0 || fail=${?}

      # note that local files get symlinked and the rm command
      # below will remove the symlink rather than the actual file
      if \
         [ -z "${F_UM_POST_UNPACK_TARBALL-}" ] || \
         ${F_UM_POST_UNPACK_TARBALL} "${fail}" "${tarball_file}" "${dst_dir}"
      then
         case "${tarball_file}" in
            "${union_tarball_container}/"*)
               autodie rm -- "${tarball_file}"
            ;;
         esac
      fi

      [ ${fail} -eq 0 ] || die "Failed to unpack ${1} (rc=${fail})" ${fail}

      shift
   done

   union_mount_finalize_tarball_container
}

## @autodie union_mount_get_and_unpack_tarball ( dst_dir, *file_uri )
##
union_mount_get_and_unpack_tarball() {
   <%%locals dst_dir< %>

   case "${dst_dir}" in
      "${union_mount_container}/"*"/"*)
         _union_mount_get_and_unpack_tarball "${@}"
      ;;
      *)
         die "union_mount_get_and_unpack_tarball(): bad dst dir: ${dst_dir}"
      ;;
   esac
}

## @autodie union_mount_get_and_unpack_tarball_forced ( dst_dir, *file_uri )
##
union_mount_get_and_unpack_tarball_forced() {
   <%%locals dst_dir< %>
   _union_mount_get_and_unpack_tarball "${@}"
}
