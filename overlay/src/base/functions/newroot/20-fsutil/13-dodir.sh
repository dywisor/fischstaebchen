## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_newroot_dodir_nonfatal() {
   <%%locals v0 %>

   newroot_prefix_fspath "${1}" && \
   _dodir_nonfatal "${v0:?}"
}

_newroot_dodir_for_file_nonfatal() {
   case "${1}" in
      ?*/*) _newroot_dodir_nonfatal "${1%/*}" ;;
      ##*)    _dodir_nonfatal "${NEWROOT}" ;;
   esac
}

newroot_dodir_nonfatal() {
   <%%argc_loop +++ _newroot_dodir_nonfatal "${1}" %>
}

newroot_dodir() {
   <%%argc_loop +++ autodie _newroot_dodir_nonfatal "${1}" %>
}

newroot_dodir_for_file_nonfatal() {
   <%%argc_loop +++ _newroot_dodir_for_file_nonfatal "${1}" %>
}

newroot_dodir_for_file() {
   <%%argc_loop +++ autodie _newroot_dodir_for_file_nonfatal "${1}" %>
}
