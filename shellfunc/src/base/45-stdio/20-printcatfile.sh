## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## cat_or_print ( *args )
cat_or_print() {
   <%%locals i s %>

   case "${1:--}" in
      '-')
         [ ${#} -gt 1 ] || { cat; return ${?}; }
      ;;

      '+++')
         @@SHIFT_OR_RET@@
         s=; for i; do s="${s} %s"; done; s="${s}\n"
         set -- "${s# }" "${@}"
      ;;

      *)
         [ ${#} -gt 1 ] || set -- '%s\n' "${1?}"
      ;;
   esac

   # shellcheck disable=SC2059
   printf "${@}"
}

write_to_file() {
   <%%locals f< %>
   dodir_for_file "${f}"
   cat_or_print "${@}" > "${f}" || die "Failed to write to '${1}'"
}

append_to_file() {
   <%%locals f< %>
   dodir_for_file "${f}"
   cat_or_print "${@}" >> "${f}" || die "Failed to append to '${1}'"
}
