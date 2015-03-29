## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

dodir_nonfatal() {
   mkdir -p -- "${@}"
}

dodir() {
   autodie mkdir -p -- "${@}"
}

_dodir_for_file_nonfatal() {
   case "${1}" in
      ?*/*) mkdir -p -- "${1%/*}" ;;
   esac
}

_dodir_for_file() {
   case "${1}" in
      ?*/*) autodie mkdir -p -- "${1%/*}" ;;
   esac
}

dodir_for_file_nonfatal() {
   <%% argc_loop +++ _dodir_for_file_nonfatal "${1}" %>
}

dodir_for_file() {
   <%% argc_loop +++ _dodir_for_file "${1}" %>
}
