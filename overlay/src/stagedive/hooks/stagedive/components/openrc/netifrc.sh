## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -- ${net_ifaces}
[ $# -gt 0 ] || return 0

netifrc_cfg="${ROOT_ETC_CONFD}/net"

if [ -f "${netifrc_cfg}" ] && \
   grep -q -- '@protect' "${netifrc_cfg}"
then
   einfo "Not creating ${netifrc_cfg}: protected file"
   return 0
fi

create_netiface_links() {
   while [ ${#} -gt 0 ]; do
      autodie ln -f -s -- net.lo "${ROOT_ETC_INITD}/net.${1}"

      @@SHIFT_OR_RET@@
   done
}

enable_services() {
   dodir "${ROOT%/}/etc/runlevels/default"

   while [ ${#} -gt 0 ]; do
      autodie ln -f -s -- "${RREL_ETC_INITD}/net.lo" \
         "${ROOT%/}/etc/runlevels/default/net.${1}"

      @@SHIFT_OR_RET@@
   done
}



{
   print_vassign modules "udhcpc"
   print_line
} > "${netifrc_cfg}" || die "Failed to create ${netifrc_cfg}"




case "${net_iftype}" in
   single)
      {
         for iface in "${@}"; do
            print_vassign "config_${iface}" "dhcp"
         done
      } >> "${netifrc_cfg}" || die "Failed to write ${netifrc_cfg}"

      create_netiface_links "${@}"
      enable_services "${@}"
   ;;

   bridge)
      {
         print_vassign bridge_br0 "${*}"
         print_vassign config_br0 "dhcp"
         print_vassign rc_net_br0_need ""

         print_line

         for iface in "${@}"; do
            print_vassign rc_net_br0_need \
               "\${rc_net_br0_need} net.${iface}"

            print_vassign "config_${iface}" null
            print_vassign "rc_net_${iface}_provide" '!net'
         done
      } >> "${netifrc_cfg}" || die "Failed to write ${netifrc_cfg}"

      create_netiface_links "${@}" br0
      enable_services br0
   ;;

   *)
      die "unsupport net_iftype: ${net_iftype}"
   ;;
esac
