## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## FIXME: remove this file and use chroot_root*() instead
chroot_newroot_nonfatal() {
   (
      cd "${NEWROOT}" && \
      exec chroot "${NEWROOT}" "${@}"
   )
}

chroot_newroot() {
   autodie chroot_newroot_nonfatal "${@}"
}

chroot_newroot_or_die() {
   chroot_newroot_nonfatal "${@}" || \
      die "failed to chroot-exec ${1:-???} in newroot (rc=${?})" ${?}
}
