## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% weakdef HAVE_SHA1 sha1sum %>
<% weakdef HAVE_MD5  md5     %>

<% if HASHFUNC_BIND_DYNAMIC=0 %>

<% define SHA1_HASH_CHECK _ "53a0acfad59379b3e050338bf9f23cfc172ee787" %>
<% define MD5_HASH_CHECK  _ "b14a7b8059d9c055954c92674ce60032" %>

## wont work due to newline:
##__strfeed_cmd_equals() { [ "$(strfeed_cmd "${2}" ${1})" = "${3}" ]; }
__strfeed_cmd_equals() { [ "$(printf "%s" "${2}" | ${1})" = "${3}" ]; }

# let's see if which commands are actually available
HAVE_SHA1=
<% if HAVE_SHA1 %>
if __strfeed_cmd_equals io_sha1 \
   @@SHA1_HASH_CHECK@@
then
   ## "_\n" => 71e74fbf7a4635debe9ca464b977604dac9c7f33
   HAVE_SHA1=@@HAVE_SHA1@@
fi

<% endif %>
HAVE_MD5=
<%if HAVE_MD5 %>
if __strfeed_cmd_equals io_md5sum \
   @@MD5_HASH_CHECK@@
then
   HAVE_MD5=@@HAVE_MD5@@
fi

<% endif %>
# bind io_hash(), hash_str() functions
##
##  prefer sha1 over md5, doesn't matter that much in terms of speed
##  (x86 is fast enough and the few ARM devices I have do sha1 in hardware - YMMV)
##
if [ -n "${HAVE_SHA1}" ]; then
HASH_TYPE=sha1
# shellcheck disable=SC2120
io_hash() { sha1sum "$@" @@NO_STDERR@@ | io_first_whitespace_field; }

elif [ -n "${HAVE_MD5}" ]; then
HASH_TYPE=md5
# shellcheck disable=SC2120
io_hash() { md5sum "$@" @@NO_STDERR@@ | io_first_whitespace_field; }

else
HASH_TYPE=
io_hash() { return @@EX_NOT_SUPPORTED@@; }
fi


<% endif %>
