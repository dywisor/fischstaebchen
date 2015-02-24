## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

DEFAULT_ROOT_PATHV="/bin /sbin /usr/bin /usr/sbin"

## __root_find_prog ( f_filter_dir, prog )
__root_find_prog() {
   _rootdir_find_prog \
      "${ROOT:?}" "${ROOT_PATHV:-${DEFAULT_ROOT_PATHV}}" "${@}"
}

## root_find_prog ( prog )
root_find_prog() {
   __root_find_prog false "${@}"
}

root_find_prog_ignore_symlink_dirs() {
   __root_find_prog test_fs_is_symlink "${@}"
}

root_has_prog() {
   <%%locals v0 v1 %>
   __root_find_prog false "${@}"
}
