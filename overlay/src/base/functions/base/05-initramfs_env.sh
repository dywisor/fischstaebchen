## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

. "@@ENVFILE@@" || exit

if [ -f "${IENV:-/}" ]; then
   . "${IENV}" || exit
fi

if [ "${NO_COLOR}" != "y" ]; then
   if test -f "${ICOLORS:-/}"; then
      ## ( . file; ) && . file || NO_COLOR=y
      . "${ICOLORS}" || exit
   else
      NO_COLOR=y
   fi
fi

if [ "${NO_COLOR}" = "y" ]; then
EINFO_COLOR=
EWARN_COLOR=
EERROR_COLOR=
__DEFAULT_COLOR=
else
EINFO_COLOR="${MSG_COLOR_GREEN}"
EWARN_COLOR="${MSG_COLOR_YELLOW}"
EERROR_COLOR="${MSG_COLOR_RED}"
__DEFAULT_COLOR="${MSG_COLOR_DEFAULT}"
fi

__debug__()   { [ "${DEBUG:-X}"   = "${SHTRUE}" ]; }
__verbose__() { [ "${VERBOSE:-X}" = "${SHTRUE}" ]; }
__quiet__()   { [ "${QUIET:-X}"   = "${SHTRUE}" ]; }
