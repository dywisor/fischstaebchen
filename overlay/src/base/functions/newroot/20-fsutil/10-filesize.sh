## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

newroot_get_filesize_m() {
   numfiles=0
   <%%argc_loop ::: _get_filesize_m "${NEWROOT}/${1#/}" || !\
      | die "failed to stat ${NEWROOT}/${1#/}" %>
}

