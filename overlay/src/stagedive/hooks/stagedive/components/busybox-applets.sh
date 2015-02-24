## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

[ -n "${ROOT_BUSYBOX?}" ] || return 0

blist="${T:?}/root_busybox_applets.list"

if \
   chroot_root_nonfatal /bin/busybox --list > "${blist}" && \
   [ -s "${blist}" ]
then

busybox_have_prog() { grep -q -- "^${1}\$" "${blist}"; }

else

ewarn "Could not get a list of busybox applets - assuming that all are available."
busybox_have_prog() { return 0; }

fi

binstall() {
   <%%locals asym=${ROOT%/}/${1#/} name=${1##*/} %>

   if test_fs_is_exe "${asym}" || root_find_prog "${name}"; then
      veinfo "Not installing busybox symlink ${1}: prog exists."

   elif busybox_have_prog "${name}"; then
      veinfo "Installing busybox symlink ${1}"

      dodir "${asym%/*}" && \
      autodie ln -s -- "${RREL_BUSYBOX}" "${asym}"

   else
      ewarn "Cannot install busybox symlink ${1}: applet not found."
   fi
}


binstall  /bin/sh
binstall  /sbin/brctl
binstall  /sbin/udhcpc
# netifrc is not compatible with busybox' ip applet
#binstall  /bin/ip
binstall  /bin/route
binstall  /bin/ifconfig


rm -f -- "${blist}"
