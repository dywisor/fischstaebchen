## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_check_is_mounted() {
   <%%locals pattern%>

   pattern="$( strip_fspath "${1}" )"

   grep -E ${2-} -- "\S+\s+${pattern:-${1}}\s+" /proc/mounts
}

is_mounted_verbose() {
   [ -n "${1-}" ] && _check_is_mounted "${1}"
}

is_mounted() {
   [ -n "${1-}" ] && _check_is_mounted "${1}" "-q"
}

is_mounted_in_newroot() {
   [ -n "${1-}" ] && _check_is_mounted "${NEWROOT}/${1#/}" "-q"
}
