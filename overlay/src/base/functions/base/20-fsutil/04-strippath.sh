## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if HAVE_COREUTILS= %>
<% weakdef PROG_REALPATH realpath %>
<% endif %>

<% if PROG_REALPATH= %>

strip_fspath()    { @@PROG_REALPATH@@ -Lms -- "${1}"; }
io_strip_fspath() { @@PROG_REALPATH@@ -Lms -- "$(cat)"; }

<% else %>

io_strip_fspath() {
   sed -r \
      -e 's,([^/])/+$,\1,' -e 's,/+,/,' \
      -e 's,/[.]/,/,' -e 's,^[/]?[.][/]?$,.,' -e 's,/[.]$,,'
}

strip_fspath() { printf "%s" "${1}" | io_strip_fspath; }

<% endif %>

get_abspath() { v0="$( strip_fspath "${@}" )" && [ -n "${v0}" ]; }
