## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

newroot_dosym_nonfatal() {
   <%%locals v0 %>

   newroot_prefix_fspath "${2:?}" && \
   create_symlink_nonfatal "${1:?}" "${v0}" "${3-}"
}

newroot_dosym() {
   <%%locals v0 %>

   newroot_prefix_fspath "${2:?}" && \
   create_symlink "${1:?}" "${v0}" "${3-}"
}
