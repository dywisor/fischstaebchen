## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

check_dir_writable() {
   if __quietly__ touch "${1}/.__initramfs_check_rw__"; then
      rm -f -- "${1}/.__initramfs_check_rw__"
      return 0
   fi

   return 1
}
