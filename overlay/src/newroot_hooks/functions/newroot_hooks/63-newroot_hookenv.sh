## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if ALLOW_NEWROOT_HOOKS %>

newroot_hook__setup_env_initramfs() {
   D="${NEWROOT}"
   IN_NEWROOT=n
}

newroot_hook__setup_env_newroot() {
   D=/
   IN_NEWROOT=y
}

newroot_hook__setup_env_do_export() {
   newroot_hook__setup_env_${1:?}

   export D
   export IN_NEWROOT
}

<% endif %>
