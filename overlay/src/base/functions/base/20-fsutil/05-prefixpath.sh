## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int prefix_fspath ( prefix, fspath, **v0! )
##
prefix_fspath() {
   <%%retvar v0%>

   [ -n "${1-}" ] && [ -n "${2-}" ] || return @@EX_USAGE@@

   case "${2}" in
      '')
         return @@EX_USAGE@@
      ;;
      //*)
         v0="${2#/}"
      ;;
      /)
         v0="${1}"
      ;;
      "${1}/"*)
         v0="${2}"
      ;;
      *)
         v0="${1%/}/${2#/}"
      ;;
   esac

   return 0
}

## int prefix_fspath_recursive ( prefix..., fspath, **v0! )
##
prefix_fspath_recursive() {
   prefix_fspath "${1-}" "${2-}" || return
   shift 2 || return @@EX_SHIFT_ERR@@

   <%%argc_loop +++ prefix_fspath "${v0}" "${1}" %>
}

## int newroot_prefix_fspath ( prefix..., fspath, **v0! )
##  IS prefix_fspath_recursive ( NEWROOT, prefix..., fspath )
##
newroot_prefix_fspath() {
   prefix_fspath_recursive "${NEWROOT%/}" "${@}"
}
