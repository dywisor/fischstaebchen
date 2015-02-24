## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

stagedive_bootstrap_basemounts() {
   : ${1:?}
   <%%locals d mp %>

   for d in /proc /dev /dev/pts; do
      mp="${1%/}/${d#/}"

      is_mounted "${mp}" || xmount_bind "${d}" "${mp}"
   done
}

stagedive_bootstrap_eject_basemounts() {
   : ${1:?}
   <%%locals d mp %>

   for d in /dev/pts /dev /proc; do
      mp="${1%/}/${d#/}"

      ! is_mounted "${mp}" || autodie umount "${mp}"
   done
}

stagedive_bootstrap_kill_chroot_processes() {
   kill_chroot_processes "${@}"
}
