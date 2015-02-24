## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

chroot_root_nonfatal() {
   (
      cd "${ROOT}" && \
      exec chroot "${ROOT}" "${@}"
   )
}

chroot_root() {
   autodie chroot_root_nonfatal "${@}"
}

chroot_root_or_die() {
   chroot_root_nonfatal "${@}" || \
      die "failed to chroot-exec ${1:-???} in ${ROOT:-???} (rc=${?})" ${?}
}
