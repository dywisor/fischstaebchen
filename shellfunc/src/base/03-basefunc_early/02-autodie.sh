## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

autodie() {
   <%%locals -i rc %>
   "${@}" && return 0 || rc=${?}

   printf "returned %d:\n  %s\n" "${rc}" "${*}"
   die "command '${*}' returned ${rc}." ${rc}
}
