## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## gen_squashfs_union_parser_code ( func_name, var_name, flagfile=, func= )
##
gen_squashfs_union_parser_code() {
   <%%varcheck 1..2 %>
   #_cmdline_gen_squashfs_union_parser_code "$@"

   print_gen_from_metascript union_mount_parser \
      F="${1}" \
      V="${2}" \
      FLAGFILE="${3-}" \
      CALLBACK="${4-}"
}

## eval_squashfs_union_parser ( func_name, var_name, flagfile=, func= )
##
eval_squashfs_union_parser() {
   <%%varcheck 1..2 %>
   <%%local pfile %>

   # more controlled that way (catch gen-code failure)
   pfile="@@ISHARE_HOOKS@@/_cmdline_parse__${1}"

   ( gen_squashfs_union_parser_code "$@"; ) > "${pfile}" || \
      die "failed to create cmdline parser code file ${pfile}"

   . "${pfile}" || \
      die "failed to load cmdline parser code file ${pfile:-%pfile%}"
}
