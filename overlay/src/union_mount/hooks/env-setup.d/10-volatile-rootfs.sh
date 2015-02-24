#!/bin/busybox ash
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -u

ishare_has_flag want-volatile-rootfs || exit 0

gen_union_mount_script _ 10-volatile_rootfs \
   VOLATILE_ROOTFS / none y n
