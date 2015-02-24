## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if ABDUCT_DIE= %>
<% define _DIEFUNC __die %>
<% else %>
<% define _DIEFUNC die %>
<% endif %>
## @noreturn die ( message=, exit_code:=@@EX_DIE@@ )
##
@@_DIEFUNC@@() {
   <%%locals die_word=died %>
   if [ -n "${1-}" ]; then
      die_word="${die_word}:"
   else
      die_word="${die_word}."
   fi

   if [ "${HAVE_MESSAGE_FUNCTIONS:-X}" = "y" ]; then
      eerror "${1-}" "${die_word}"
   else
      printf "%s\n" "${die_word}${1-}" 1>&2
   fi

   if [ -n "${DIE_DBGFILE-}" ]; then
      printf "%s\n" "${1:-%unknown%}" >> "${DIE_DBGFILE}" || @@NOP@@
   fi

   exit ${2:-@@EX_DIE@@}
}

<% if ABDUCT_DIE= %>
die() {
   ${DIE_FUNCTION:-@@_DIEFUNC@@} "${@}"
}
<% endif %>
