## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

rootpass="${CMDLINE_ROOTPASS:-idkfa}"
if [ -z "${CMDLINE_ROOTPASS-}" ]; then
   einfo "Setting root password to '${rootpass}'"
else
   einfo "Setting root password (from cmdline)"
fi

root_passwd root "${rootpass}" || \
   die "Failed to set root password in ${ROOT:-???}."
