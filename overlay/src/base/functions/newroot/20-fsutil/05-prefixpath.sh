## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int newroot_prefix_fspath ( prefix..., fspath, **v0! )
##  IS prefix_fspath_recursive ( NEWROOT, prefix..., fspath )
##
newroot_prefix_fspath() {
   prefix_fspath_recursive "${NEWROOT%/}" "${@}"
}
