## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

DEFAULT_NEWROOT_PATHV="/bin /sbin /usr/bin /usr/sbin"

## __newroot_find_prog ( f_filter_dir, prog )
__newroot_find_prog() {
   _rootdir_find_prog \
      "${NEWROOT}" "${NEWROOT_PATHV:-${DEFAULT_NEWROOT_PATHV}}" "${@}"
}

## newroot_find_prog ( prog )
newroot_find_prog() {
   __newroot_find_prog false "${@}"
}

newroot_find_prog_ignore_symlink_dirs() {
   __newroot_find_prog test_fs_is_symlink "${@}"
}

newroot_has_prog() {
   <%%locals v0 v1 %>
   __newroot_find_prog false "${@}"
}
