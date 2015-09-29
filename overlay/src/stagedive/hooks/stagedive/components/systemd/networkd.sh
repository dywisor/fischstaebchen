## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -- ${net_ifaces}
[ $# -gt 0 ] || return 0

NETWORKD="${ROOT_ETC_SYSTEMD}/network"


write_systemd_network_file() {
write_to_file "${NETWORKD}/${2:-40}-${1:?}.network" << EOF
[Match]
Name=${1:?}

[Network]
Description=stagedive-configured interface
DHCP=both
EOF
}

write_systemd_bridge_file() {
write_to_file "${NETWORKD}/${2:-30}-${1:?}.netdev" << EOF
[NetDev]
Description=stagedive-configured interface
Name=${1:?}
Kind=bridge
EOF
}

write_systemd_bridge_member_file() {
write_to_file "${NETWORKD}/${3:-35}-${2:?}.network" << EOF
[Match]
Name=${2:?}

[Network]
Description=stagedive-configured interface
Bridge=${1:?}
EOF
}


case "${net_iftype}" in
   single)
      <%%foreach iface ::: autodie write_systemd_network_file "${1:?}" %>
   ;;

   bridge)
      autodie write_systemd_bridge_file br0
      <%%foreach iface ::: !\
         | autodie write_systemd_bridge_member_file br0 "${iface}" %>
      autodie write_systemd_network_file br0
   ;;

   *)
      die "unsupported net_iftype: ${net_iftype}"
   ;;
esac

systemd_enable_unit systemd-networkd.service
