#!@@XSHELL@@
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -u

initramfs_fw_dir="/lib/firmware"
initramfs_kmod_root="/lib/modules"

initramfs_kmod_dir=
newroot_kmod_dir=

want_xfer_kmod=
if [ -z "${NEWROOT_MODULES_DIR-}" ]; then
   eerror "cannot copy module files: NEWROOT_MODULES_DIR is not set."

elif kver="$(uname -r @@NO_STDERR@@)" && [ -n "${kver}" ]; then
   initramfs_kmod_dir="${initramfs_kmod_root}/${kver}"
   newroot_kmod_dir="${NEWROOT_MODULES_DIR}/${kver}"

   if \
      [ -d "${initramfs_kmod_dir}" ] && \
      [ ! -d "${newroot_kmod_dir}" ]
   then
      want_xfer_kmod=y
      einfo "found kernel modules"
   fi
fi

want_xfer_fw=
if [  -z "${NEWROOT_FIRMWARE_DIR-}" ]; then
   eerror "cannot copy firmware files: NEWROOT_FIRMWARE_DIR is not set."

elif \
   [ -d "${initramfs_fw_dir}" ] && \
   [ ! -d "${NEWROOT_FIRMWARE_DIR}" ]
then
   einfo "found kernel firmware files"
   want_xfer_fw=y
fi

transfer_modules_and_firmware() {
   <%%local -i fail=0 %>

   if [ -n "${want_xfer_fw}" ]; then
      einfo "Copying kernel firmware files to newroot"
      copytree \
         "${initramfs_fw_dir}" "${NEWROOT_FIRMWARE_DIR}" || fail=0 ## non-fatal for now
   fi

   if [ -n "${want_xfer_kmod}" ]; then
      einfo "Copying kernel modules to newroot"
      copytree \
      "${initramfs_kmod_dir}" "${newroot_kmod_dir}" || fail=0 ## non-fatal
   fi

   return ${fail}
}


if [ -n "${want_xfer_fw}${want_xfer_kmod}" ]; then
   # shellcheck disable=SC2015
   get_newroot_rw_nonfatal && \
   transfer_modules_and_firmware || \
      eerror "failed to transfer kernel modules/firmware!"
fi
