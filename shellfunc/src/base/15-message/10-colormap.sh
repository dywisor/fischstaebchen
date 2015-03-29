## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if [ -z "${HAVE_ICOLORS-}" ]; then

# FIXME: taken from /run/initramfs/colors.sh,
#         should escape the color codes properly.
MSG_COLOR_DEFAULT='[0m'
MSG_COLOR_BLACK='[1;30m'
MSG_COLOR_RED='[1;31m'
MSG_COLOR_GREEN='[1;32m'
MSG_COLOR_YELLOW='[1;33m'
MSG_COLOR_BLUE='[1;34m'
MSG_COLOR_MAGENTA='[1;35m'
MSG_COLOR_CYAN='[1;36m'
MSG_COLOR_WHITE='[1;37m'
MSG_COLOR_BLACK_NB='[0;30m'
MSG_COLOR_RED_NB='[0;31m'
MSG_COLOR_GREEN_NB='[0;32m'
MSG_COLOR_YELLOW_NB='[0;33m'
MSG_COLOR_BLUE_NB='[0;34m'
MSG_COLOR_MAGENTA_NB='[0;35m'
MSG_COLOR_CYAN_NB='[0;36m'
MSG_COLOR_WHITE_NB='[0;37m'
MSG_COLOR_BLACK_BG='[1;37;100m'
MSG_COLOR_RED_BG='[1;37;101m'
MSG_COLOR_GREEN_BG='[1;37;102m'
MSG_COLOR_YELLOW_BG='[1;37;103m'
MSG_COLOR_BLUE_BG='[1;37;104m'
MSG_COLOR_MAGENTA_BG='[1;37;105m'
MSG_COLOR_CYAN_BG='[1;37;106m'
MSG_COLOR_WHITE_BG='[1;30;107m'

fi # HAVE_ICOLORS
