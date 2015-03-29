## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

get_any_file() {
   <%%retvar v0%>
   while [ $# -gt 0 ]; do
      if test_fs_is_file "${1}"; then
         v0="${1}"
         return 0
      fi
      shift
   done

   return 1
}

get_any_file_from() {
   <%%retvars v0 v1 %>
   <%%locals root<=${1?} root=${root%/} %>

   while [ $# -gt 0 ]; do
      # assuming that %root is not a file...
      if test_fs_is_file "${root}/${1#/}"; then
         v0="${1}"
         v1="${root}/${1#/}"
         return 0
      fi
      shift
   done

   return 1
}

get_any_file_oldnew_from() {
   <%%retvars oldnew_match_base oldnew_match_suffix %>
   <%%locals root<=${1?} root=${root%/} suffix %>

   while [ $# -gt 0 ]; do
      if [ -n "${1}" ]; then
         for suffix in ".new" "" ".old"; do
            if get_any_file_from "${root}" "${1}${suffix}"; then
               oldnew_match_base="${1}"
               oldnew_match_suffix="${suffix}"
               return 0
            fi
         done
      fi

      shift
   done

   return 1
}
