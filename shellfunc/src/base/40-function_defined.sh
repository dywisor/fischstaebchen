## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

function_defined() {
   : ${1:?}

   case "$( LANG=C LC_ALL=C command -V "${1}" @@NO_STDERR@@ )" in
      "${1} is a "*" function"*)
         return 0
      ;;
   esac

   return 1
}

call_function_if_defined() {
   ! function_defined "${1:?}" || "${@}"
}

autodie_call_function_if_defined() {
   ! function_defined "${1:?}" || autodie "${@}"
}
