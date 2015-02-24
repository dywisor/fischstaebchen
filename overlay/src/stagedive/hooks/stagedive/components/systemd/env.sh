## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## __systemd_enable_unit_init (
##    libdir_unit_name, confdir_unit_name:=%libdir_unit_name,
##    target_dirname:="multi-user.target.wants"
## )
##
__systemd_enable_unit_init() {
   ##STUB
   <%%varcheck 1 %>

   autodie check_fspath_configured "${RREL_LIB_SYSTEMD_SYSTEM}"
   autodie check_fspath_configured "${RREL_ETC_SYSTEMD_SYSTEM}"

   unit_name="${1##*/}"
   unit_file="${RREL_LIB_SYSTEMD_SYSTEM}/${unit_name}"
   target_dir="${ROOT_ETC_SYSTEMD_SYSTEM}/${3:-multi-user.target.wants}"
   unit_link="${target_dir}/${2:-${unit_name}}"
}

__systemd_enable_unit_enable() {
   einfo "Enabling systemd unit ${unit_name}"
   dodir "${target_dir}"
   autodie ln -f -s -- "${unit_file}" "${unit_link}"
}

systemd_enable_unit() {
   <%%locals unit_name unit_file unit_link target_dir %>
   __systemd_enable_unit_init "${@}"
   __systemd_enable_unit_enable
}

systemd_enable_unit_ifexist() {
   <%%locals unit_name unit_file unit_link target_dir %>
   __systemd_enable_unit_init "${@}"

   if [ -f "${ROOT%/}/${unit_file#/}" ]; then
      __systemd_enable_unit_enable
   else
      einfo "Not enabling systemd unit ${unit_name}: not found."
   fi
}
