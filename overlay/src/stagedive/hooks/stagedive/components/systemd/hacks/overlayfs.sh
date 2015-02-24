## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

case "${UNION_MOUNT_FSTYPE:-X}" in
   overlay|overlayfs)
      @@NOP@@
   ;;
   *)
      return 0
   ;;
esac


if [ -s "${ROOT%/}/etc/machine-id" ]; then
   ## https://bugs.launchpad.net/ubuntu/+source/systemd/+bug/1411140
   s="${ROOT_ETC_SYSTEMD_SYSTEM}/systemd-machine-id-commit.service"

   fs_remove "${s}"
   autodie ln -s -- "/dev/null" "${s}"
fi
