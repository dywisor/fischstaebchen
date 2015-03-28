## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if HAVE_SHA1=1 %>
# shellcheck disable=SC2120
io_sha1()     { sha1sum "$@" @@NO_STDERR@@ | io_first_whitespace_field; }
# shellcheck disable=SC2119
str_to_sha1() { sha1="$(printf "%s" "${1}" | io_sha1)"; [ -n "${sha1}" ]; }
<% endif %>
