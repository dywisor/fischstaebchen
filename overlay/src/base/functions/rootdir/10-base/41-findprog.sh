## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _rootdir_find_prog ( root, pathv, f_filter_dir, prog, **v0!, **v1! )
##
##  Does not support glob expressions in %pathv.
##  In contrast to the %PATH variable, %pathv must be a space-separated list.
##
_rootdir_find_prog() {
   <%%retvar v0 v1 %>
   <%%varcheck 2:? 3:? 4:? %>
   <%%locals root_prefix prog_name path_dir_rel path_dir_abs %>

   case "${1:?}" in
      /) root_prefix=/ ;;
      *) root_prefix="${1%/}/" ;;
   esac

   case "${4}" in
      /*)
         prog_name="${root_prefix}${4#/}"
         if _test_fs_is_exe_or_broken_sym "${prog_name}"; then
            if ! ${3:?} "${prog_name%/*}"; then
               v0="${prog_name}"
               v1="/${4#/}"
               return 0
            fi
         fi
         prog_name="${4##*/}"
      ;;
      */*)
         return @@EX_USAGE@@
      ;;
      *)
         prog_name="${4}"
      ;;
   esac

   for path_dir_rel in ${2?}; do
      case "${path_dir_rel}" in
         /)
            path_dir_abs="${root_prefix}"
         ;;
         *)
            path_dir_abs="${root_prefix}${path_dir_rel#/}"
         ;;
      esac

      if ${3:?} "${path_dir_abs}"; then
         @@NOP@@

      elif _test_fs_is_exe_or_broken_sym "${path_dir_abs}/${prog_name}"; then
         v0="${path_dir_abs}/${prog_name}"
         v1="${path_dir_rel}/${prog_name}"
         return 0
      fi
   done

   return 1
}

## int _rootdir_has_prog (...)
_rootdir_has_prog() {
   <%%locals v0 v1 %>
   _rootdir_find_prog "${@}"
}
