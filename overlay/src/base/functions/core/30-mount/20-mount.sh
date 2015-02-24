## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## xmount ( mp, fs, fstype:="auto", opts:="defaults" )
xmount() {
   autodie mkdir -p -- "${1}" && \
   autodie mount -t "${3:-auto}" -o "${4:-defaults}" "${2}" "${1}"
}

xremount() {
   autodie mount -o remount${2:+,${2-}} "${1:?}"
}

xremount_ro() {
   autodie mount -o remount,ro "${1}"
}

xremount_rw() {
   autodie mount -o remount,rw "${1}"
}

xmount_move() {
   autodie mkdir -p -- "${2}" && \
   autodie mount --move "${1}" "${2}"
}

xmount_bind() {
   autodie mkdir -p -- "${2}" && \
   autodie mount --bind "${1}" "${2}"
}

xmount_bind_ro() {
   xmount_bind "${1}" "${2}" && \
   xremount "${2}" bind,ro
}

xmount_if_not_mounted() {
   ! is_mounted "${1}" || return 0
   xmount "$@"
}

xmount_newroot() {
   <%%locals mp<=${NEWROOT}/${1#/}%>
   xmount "${mp}" "$@"
}

xmount_newroot_if_not_mounted() {
   <%%locals mp<=${NEWROOT}/${1#/}%>
   xmount_if_not_mounted "${mp}" "$@"
}

xmount_mnt_container() {
   xmount_if_not_mounted "${1}" mntfs tmpfs "${MNT_CONTAINER_OPTS}"
}
