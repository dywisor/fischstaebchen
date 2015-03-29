## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## ~int initramfs_logfile_printf ( ..., **INITRAMFS_LOGFILE )
## ~int initramfs_logfile_write ( message, header )
##
##  The logfile has to be created by /init
##   (obviously misbehaves if any hook deletes $INITRAMFS_LOGFILE)
##

<% if DYNAMIC_LOGFILE_FUNCTIONS=0 %>
# shellcheck disable=SC2059
initramfs_logfile_printf() {
   [ ! -f "${INITRAMFS_LOGFILE:-/}" ] || \
   printf "${@}" >> "${INITRAMFS_LOGFILE}"
}

initramfs_logfile_write() {
   initramfs_logfile_printf "%s%s%s\n" \
      "[hooks:${PHASE:-???}]" "${2:+ ${2}:}" "${1:+ ${1}}"
}

MESSAGE_LOG_FUNC=initramfs_logfile_write

<% else %>
if [ -f "${INITRAMFS_LOGFILE:-/}" ]; then
# shellcheck disable=SC2059
initramfs_logfile_printf() { printf "${@}" >> "${INITRAMFS_LOGFILE}"; }

initramfs_logfile_write() {
   initramfs_logfile_printf "%s%s%s\n" \
      "[hooks:${PHASE:-???}]" "${2:+ ${2}:}" "${1:+ ${1}}"
}

MESSAGE_LOG_FUNC=initramfs_logfile_write

else
initramfs_logfile_printf() { return 0; }
initramfs_logfile_write()  { return 0; }

MESSAGE_LOG_FUNC=true
fi
<% endif %>
