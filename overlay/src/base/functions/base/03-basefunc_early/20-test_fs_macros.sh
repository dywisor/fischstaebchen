## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

test_fs_is_file() {
   [ -n "${1-}" ] && [ -f "${1}" ]
}

test_fs_is_dir() {
   [ -n "${1-}" ] && [ -d "${1}" ]
}

_test_fs_lexists() { [ -e "${1}" ] || [ -h "${1}" ]; }
test_fs_lexists()  { [ -n "${1-}" ] && _test_fs_lexists "${1}"; }

## FIXME DEPRECATED: use lexists()
test_fs_exists() { [ -n "${1-}" ] && _test_fs_lexists "${1}"; }

_test_fs_is_exe() {
   [ -x "${1}" ] && [ -f "${1}" ]
}

_test_fs_is_exe_or_broken_sym() {
   if [ -e "${1}" ]; then
      if [ -x "${1}" ] && [ -f "${1}" ]; then return 0; fi
   elif [ -h "${1}" ]; then
      return 0
   fi
   return 1
}

test_fs_is_exe() {
   [ -n "${1-}" ] && _test_fs_is_exe_or_broken_sym "${1}"
}

test_fs_is_symlink() {
   [ -n "${1-}" ] && [ -h "${1}" ]
}

test_fs_is_real_file() {
   [ -n "${1-}" ] && [ -f "${1}" ] && [ ! -h "${1}" ]
}

test_fs_is_real_dir() {
   [ -n "${1-}" ] && [ -d "${1}" ] && [ ! -h "${1}" ]
}
