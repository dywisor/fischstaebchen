## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## __message_emitter (
##    fmt_end, logfile_header, header_color, message, header
## )
##
__message_emitter() {
   logfile_write "${4}" "${2}"

   if [ -n "${5-X}" ]; then
      printf "%s%s%s%s${1-}" \
         "${3}" "${5-*}" "${__DEFAULT_COLOR}" "${4:+ }${4}"
   else
      printf "%s${1-}" "${4}"
   fi
}
<%define MSGL __message_emitter  "\n" %>
<%define MSGN __message_emitter    "" %>

<%if DYNAMIC_MESSAGE_FUNCTIONS=0 %>
einfo()    { __quiet__ || @@MSGL@@  "info" "${EINFO_COLOR}"   "$@"; }
einfon()   { __quiet__ || @@MSGN@@  "info" "${EINFO_COLOR}"   "$@"; }
veinfo()   { ! __verbose__ || einfo   "$@"; }
veinfon()  { ! __verbose__ || einfon  "$@"; }
vveinfo()  { ! __debug__   || einfo   "$@"; }
vveinfon() { ! __debug__   || einfon  "$@"; }

<% else %>
if __quiet__; then
einfo()  { return 0; }
einfon() { return 0; }
else
einfo()   { @@MSGL@@  "info" "${EINFO_COLOR}"   "$@"; }
einfon()  { @@MSGN@@  "info" "${EINFO_COLOR}"   "$@"; }
fi

if __verbose__; then
veinfo()  { einfo   "$@"; }
veinfon() { einfon  "$@"; }
else
veinfo()  { return 0; }
veinfon() { return 0; }
fi

if __debug__; then
vveinfo()  { einfo   "$@"; }
vveinfon() { einfon  "$@"; }
else
vveinfo()  { return 0; }
vveinfon() { return 0; }
fi

<%endif%>

ewarn()   { @@MSGL@@  "warning" "${EWARN_COLOR}"   "$@" 1>&2; }
ewarnn()  { @@MSGN@@  "warning" "${EWARN_COLOR}"   "$@" 1>&2; }
eerror()  { @@MSGL@@  "error"   "${EERROR_COLOR}"  "$@" 1>&2; }
eerrorn() { @@MSGN@@  "error"   "${EERROR_COLOR}"  "$@" 1>&2; }


HAVE_MESSAGE_FUNCTIONS=y
