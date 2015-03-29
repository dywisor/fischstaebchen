## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if HAVE_MD5=1 %>
# shellcheck disable=SC2120
io_md5sum()   { md5sum "$@" @@NO_STDERR@@ | io_first_whitespace_field; }
# shellcheck disable=SC2119
str_to_md5()  { md5="$(printf "%s" "${1}" | io_md5sum)"; [ -n "${md5}" ]; }
<% endif %>
