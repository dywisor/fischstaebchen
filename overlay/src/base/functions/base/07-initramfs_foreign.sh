## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if FOREIGN_INITRAMFS=0 %>
case "${INITRAMFS_LOGFILE=}" in
   ?*/*)
      autodie mkdir -p -- "${INITRAMFS_LOGFILE%/*}"
   ;;
esac
<% endif %>
