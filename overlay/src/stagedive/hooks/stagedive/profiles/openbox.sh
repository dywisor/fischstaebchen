## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

stagedive_inherit  _live

# some distros use conf.d for different purposes,
#  check for [/usr]/lib/rc as well
# FIXME: make this configurable (%ROOT_HAS_OPENRC in <profile>.sh)
if \
   check_fspath_configured "${RREL_ETC_CONFD}" && \
   {
      test_fs_lexists "${ROOT%/}/lib/rc" || \
      test_fs_lexists "${ROOT%/}/usr/lib/rc"
   }
then
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
