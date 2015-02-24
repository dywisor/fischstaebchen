## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _import_to_dir_prepare_dst_dir ( dst_dir )
##
_import_to_dir_prepare_dst_dir() {
   if test -h "${1}"; then
      vveinfo "import_to_dir: dropping symlink ${1}"
      if ! rm -- "${1}"; then
         eerror "import_to_dir: failed to remove symlink ${1}"
         return 1
      fi

   elif test -d "${1}"; then
      @@NOP@@

   elif test -e "${1}"; then
      eerror "import_to_dir: cannot create destdir ${1}: exists (not a dir)"
      return 1

   elif ! @@DBGTRACE_CMD@@ mkdir -p -- "${1}"; then
      eerror "import_to_dir: failed to create destdir ${1}"
      return 1
   fi

   return 0
}

## int _import_to_dir_prepare_dst_dir_move_old ( dst_dir, ~~~bak~~~, **v0! )
##
##  not implemented.
##
