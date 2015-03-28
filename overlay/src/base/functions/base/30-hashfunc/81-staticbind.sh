## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% weakdef HAVE_SHA1 sha1sum %>
<% weakdef HAVE_MD5  md5     %>
<% if !HASHFUNC_BIND_DYNAMIC=0 %>
## using static feature definitions
<% if HAVE_SHA1 %>
HAVE_SHA1=@@HAVE_SHA1@@
<% endif %>
<% if HAVE_MD5 %>
HAVE_MD5=@@HAVE_MD5@@
<% endif %>

<% if HAVE_SHA1 %>
HASH_TYPE=sha1
# shellcheck disable=SC2120
io_hash() { sha1sum "$@" @@NO_STDERR@@ | io_first_whitespace_field; }
<% elseif HAVE_MD5 %>
HASH_TYPE=md5
# shellcheck disable=SC2120
io_hash() { md5sum "$@" @@NO_STDERR@@ | io_first_whitespace_field; }
<% else %>
HASH_TYPE=
io_hash() { return @@EX_NOT_SUPPORTED@@; }
<% endif %>
<% endif %>
