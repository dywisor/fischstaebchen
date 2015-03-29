## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if PROG_FILESIZE= %>
_get_filesize_m() {
   : ${total_size:=0}

   if v0="$( @@PROG_FILESIZE@@ "${1}" )" && [ -n "${v0}" ]; then
      <%%inc numfiles %>
      <%%inc total_size v0 %>
      return 0
   else
      v0=0
      return 1
   fi
}
<% else %>
_get_filesize_m() {
   die "_get_filesize_m() is not available." @@EX_NOT_SUPPORTED@@
}
<% endif %>

get_filesize_m() {
   numfiles=0
   <%%argc_loop ::: _get_filesize_m "${1}" || die "failed to stat ${1}" %>
}
