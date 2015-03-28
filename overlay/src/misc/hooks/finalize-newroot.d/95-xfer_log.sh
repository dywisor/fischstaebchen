#!@@XSHELL@@
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -u

if [ -f "${INITRAMFS_LOGFILE:-/}" ]; then
   did_copy=

   for dst in \
      "${NEWROOT}/var/log/initramfs.log" \
      "${NEWROOT}/tmp/initramfs.log" \
      "${NEWROOT}/status/initramfs.log"
   do
      if cp -Lf -- "${INITRAMFS_LOGFILE}" "${dst}" @@NO_STDERR@@; then
         did_copy="${dst}"
         chmod 0444 "${dst}"
         break
      fi
   done
fi
