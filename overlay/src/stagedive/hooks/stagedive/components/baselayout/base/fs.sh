## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

dodirif() { check_fspath_configured "${1}" || dodir "${1}"; }

dodir "${ROOT%/}/etc"

dodirif "${ROOT_ETC_CONFD}"
dodirif "${ROOT_ETC_INITD}"

if check_fspath_configured "${ROOT_ETC_SYSTEMD_SYSTEM}"; then
   dodir "${ROOT_ETC_SYSTEMD_SYSTEM}"
   dodir "${ROOT_ETC_SYSTEMD}/network"
   nullsym_if_nexist "${ROOT_ETC_SYSTEMD}/network/99-default.link"
fi

if check_fspath_configured "${ROOT_ETC_UDEV_RULES}"; then
   dodir "${ROOT_ETC_UDEV_RULES}"
   nullsym_if_nexist "${ROOT%/}/etc/udev/rules.d/80-net-name-slot.rules"
   nullsym_if_nexist "${ROOT%/}/etc/udev/rules.d/80-net-setup-link.rules"
fi
