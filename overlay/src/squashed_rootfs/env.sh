## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

NEWROOT_USR_ROOTFS_REL="usr/rootfs"
NEWROOT_USR_ROOTFS="${NEWROOT}/${NEWROOT_USR_ROOTFS_REL}"

## FIXME: should not be set here
# none, auto, symlink, bind[_ro], copy
USR_ROOTFS_METHOD=auto
