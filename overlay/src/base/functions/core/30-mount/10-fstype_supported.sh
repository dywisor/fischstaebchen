## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

check_fstype_supported() {
   < /proc/filesystems  grep -E -q  -- '^(.*\s)'"${1}"'$'
}
