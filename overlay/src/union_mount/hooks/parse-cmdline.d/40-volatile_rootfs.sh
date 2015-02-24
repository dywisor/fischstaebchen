#!/bin/sh
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

VOLATILE_ROOTFS_MEM=

eval_squashfs_union_parser volatile_rootfs \
   VOLATILE_ROOTFS want-volatile-rootfs
