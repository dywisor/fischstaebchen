## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

unset -v DEFAULT_EINFO_COLOR
unset -v DEFAULT_EWARN_COLOR
unset -v DEFAULT_EERROR_COLOR

message_disable_color() {
   EINFO_COLOR=
   EWARN_COLOR=
   EERROR_COLOR=
   __DEFAULT_COLOR=
}

message_enable_color() {
   : ${DEFAULT_EINFO_COLOR="${MSG_COLOR_GREEN}"}
   : ${DEFAULT_EWARN_COLOR="${MSG_COLOR_YELLOW}"}
   : ${DEFAULT_EERROR_COLOR="${MSG_COLOR_RED}"}

   EINFO_COLOR="${DEFAULT_EINFO_COLOR}"
   EWARN_COLOR="${DEFAULT_EWARN_COLOR}"
   EERROR_COLOR="${DEFAULT_EERROR_COLOR}"
   __DEFAULT_COLOR="${MSG_COLOR_DEFAULT}"
}

if [ "${NO_COLOR:-X}" = "y" ]; then
   message_disable_color
else
   message_enable_color
fi
