## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## ~int logfile_printf ( ..., **INITRAMFS_LOGFILE )
## ~int logfile_write ( message, header )
##
##  The logfile has to be created by /init
##   (obviously misbehaves if any hook deletes $INITRAMFS_LOGFILE)
##

<% if DYNAMIC_LOGFILE_FUNCTIONS=0 %>
# shellcheck disable=SC2059
logfile_printf() {
   [ ! -f "${INITRAMFS_LOGFILE:-/}" ] || \
   printf "${@}" >> "${INITRAMFS_LOGFILE}"
}

logfile_write() {
   logfile_printf "%s%s%s\n" \
      "[hooks:${PHASE:-???}]" "${2:+ ${2}:}" "${1:+ ${1}}"
}

MESSAGE_LOG_FUNC=logfile_write

<% else %>
if [ -f "${INITRAMFS_LOGFILE:-/}" ]; then
# shellcheck disable=SC2059
logfile_printf() { printf "${@}" >> "${INITRAMFS_LOGFILE}"; }

logfile_write() {
   logfile_printf "%s%s%s\n" \
      "[hooks:${PHASE:-???}]" "${2:+ ${2}:}" "${1:+ ${1}}"
}

MESSAGE_LOG_FUNC=logfile_write

else
logfile_printf() { return 0; }
logfile_write()  { return 0; }

MESSAGE_LOG_FUNC=true
fi
<% endif %>
