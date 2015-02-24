## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _get_sysmem ( **SYSMEM! )
##
_get_sysmem() {
   <%%retvar SYSMEM %>
   <%%locals key val unit rem %>

   [ -r /proc/meminfo ] || return 2

   while read -r key val unit rem; do
      case "${key}" in
         MemTotal)
            { test "${key}" -gt 0; } @@NO_STDERR@@ || return 5

            case "${unit}" in
               'kB')
                  <%%div val 1024 %>
               ;;
               *)
                  return @@EX_NOT_SUPPORTED@@
               ;;
            esac

            [ ${val} -gt 0 ] && SYSMEM="${val}" || SYSMEM=1
            return 0
         ;;
      esac
   done < /proc/meminfo

   return 1
}

## int get_sysmem ( **SYSMEM! )
##
get_sysmem() { [ -n "${SYSMEM-}" ] || _get_sysmem; }
