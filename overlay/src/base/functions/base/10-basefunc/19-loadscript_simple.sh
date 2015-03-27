## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int __loadscript_simple_set_file_vars ( file_arg )
__loadscript_simple_set_file_vars() {
   case "${1:?}" in
      */)
         return @@EX_USAGE@@
      ;;
      */*)
         __DIR__="${1%/*}"
         __FILE__="${1}"
         __FNAME__="${__FILE__##*/}"
      ;;
      *)
         __DIR__="${PWD:?}"
         __FILE__="${__DIR__}/${1}"
         __FNAME__="${1}"
      ;;
   esac
   __NAME__="${__FNAME__%.*}"
}

## int loadscript_simple ( file_arg )
##
loadscript_simple() {
   <%%locals __FILE__ __FNAME__ __NAME__ __DIR__ %>

   __loadscript_simple_set_file_vars "${1:?}" && shift || return ${?}
   ## note that "test -f" filters out devices, pipes,
   ## but also "/proc/self/fd/0" (stdin)
   [ -f "${__FILE__:?}" ] || return 1

   . "${__FILE__}"
}
