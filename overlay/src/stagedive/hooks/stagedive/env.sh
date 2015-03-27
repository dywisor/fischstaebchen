## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

case "${ROOT-}" in
   '')
      die "bad invocation - use _stagedive_run_setup_hooks()!"
      return
   ;;
   '/')
      die "ROOT must not be ${ROOT}!"
   ;;
esac


root_detect_progs() {
   <%%locals v0 v1 %>

   if root_find_prog busybox; then
      ROOT_BUSYBOX="${v0:?}"
      RREL_BUSYBOX="${v1:?}"
   else
      ROOT_BUSYBOX=
      RREL_BUSYBOX=
   fi

   ROOT_PROG_PASSWD=
   if root_find_prog passwd; then
      ROOT_PROG_PASSWD="${v1}"
   elif [ -n "${ROOT_BUSYBOX}" ]; then
      ROOT_PROG_PASSWD="${RREL_BUSYBOX?} passwd"
   fi
}

autodie root_detect_fspaths
autodie root_detect_progs

_root_passwd() {
   <%%varcheck 1:? 2? %>
chroot_root_nonfatal ${ROOT_PROG_PASSWD:-passwd} "${1}" << EOF
${2}
${2}
${2}
EOF
}

root_passwd() {
   _root_passwd "${@}" @@QUIET@@ || _root_passwd "${@}"
}



netcfg_file="${ISHARE_CFG}/network_interfaces.sh"
[ -f "${netcfg_file}" ] || autodie net-setup
autodie test -f "${netcfg_file}"

net_iftype=
net_ifaces=
net_ip4_addr=
loadscript_simple_or_die "${netcfg_file}"

{
   {
      hostname="$(hostname @@NO_STDERR@@)" && \
      case "${hostname}" in
         ''|'(none)') false ;;
      esac
   } || \
   {
      { read -r hostname DONT_CARE < /etc/hostname; } @@NO_STDERR@@ && \
      [ -n "${hostname}" ]
   }
} || hostname=

case "${hostname}" in
   ''|'(none)'|none|localhost|*initramfs*|init|*-init)
      if [ -n "${stagedive_default_hostname-}" ]; then
         hostname="${stagedive_default_hostname}"

      else
         <%%v0_strfeed stagedive-${STAGEDIVE_TYPE%%-*} !\
            | sed -r -e 's=_=-=g' -e 's=[^a-z0-9\-]==g' %>
         hostname="${v0:-stagedive}"
      fi
   ;;
esac
