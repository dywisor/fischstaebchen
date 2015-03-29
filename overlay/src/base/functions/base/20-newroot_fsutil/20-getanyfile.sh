## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

newroot_get_any_file() {
   get_any_file_from "${NEWROOT}" "$@"
}

newroot_get_any_file_oldnew() {
   get_any_file_oldnew_from "${NEWROOT}" "$@"
}
