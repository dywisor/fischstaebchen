## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if USE_QWHICH_HASH= %>
qwhich() { hash "${1}" @@NO_STDERR@@; }
<% elseif USE_QWHICH_COMMAND= %>
qwhich() { command -v "${1}" @@QUIET@@; }
<% else %>
if hash true @@QUIET@@; then
qwhich() { hash "${1}" @@NO_STDERR@@; }
elif command -v true @@QUIET@@; then
qwhich() { command -v "${1}" @@QUIET@@; }
elif type true @@QUIET@@; then
qwhich() { type "${1}" @@QUIET@@; }
else
qwhich() { which "${1}" @@QUIET@@; }
fi
<% endif %>
