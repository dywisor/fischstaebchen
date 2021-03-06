#!/bin/sh
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -u

ishare_has_flag want-squashed-usr || exit 0

gen_union_mount_script post 30-squashed_usr USR_SFS /usr
