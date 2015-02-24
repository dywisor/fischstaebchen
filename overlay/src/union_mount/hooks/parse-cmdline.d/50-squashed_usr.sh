#!/bin/sh
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

USR_SFS_FILE=auto

eval_squashfs_union_parser squashed_usr \
   USR_SFS want-squashed-usr

parse_squashed_usr_done() {
   : ${USR_SFS_FILE:=auto}

   _parse_squashed_usr_done
}
