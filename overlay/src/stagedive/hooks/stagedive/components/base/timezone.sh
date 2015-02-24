## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

[ "${STAGEDIVE_CFG_TIMEZONE}" != "none" ] || return 0

if [ -n "${STAGEDIVE_CFG_TIMEZONE-}" ]; then
   tzfile="${ROOT%/}/usr/share/zoneinfo/${STAGEDIVE_CFG_TIMEZONE}"

   if [ -e "${tzfile}" ] || [ -h "${tzfile}" ]; then
      rm -f -- "${ROOT%/}/etc/localtime"
      autodie ln -s -- \
         "/usr/share/zoneinfo/${STAGEDIVE_CFG_TIMEZONE}" \
         "${ROOT%/}/etc/localtime"

   else
      ewarn "Skipping timezone setup: tzfile is missing (${tzfile})"
   fi
fi
