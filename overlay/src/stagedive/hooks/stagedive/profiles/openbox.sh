## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

stagedive_inherit  _live

if check_fspath_configured "${RREL_ETC_CONFD}"; then
stagedive_inherit  _openrc_live
fi

if check_fspath_configured "${RREL_ETC_SYSTEMD}"; then
stagedive_inherit  _systemd_live
fi


stagedive_want  lightdm/autologin

SYSTEMD_AUTOLOGIN_TTYS="tty2 tty3"
LIGHTDM_AUTOLOGIN_USER="${STAGEDIVE_MKUSER_NAME:?}"
LIGHTDM_AUTOLOGIN_SESSION=openbox

STAGEDIVE_MKUSER_GROUPS="${STAGEDIVE_MKUSER_GROUPS:?},autologin"
STAGEDIVE_MKUSER_SETUP_SUDO=y
