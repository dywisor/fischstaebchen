## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## gen_union_mount_script (
##    phase, script_name,
##    varname_prefix, newroot_mountpoint_rel,
##    auto_files="default", is_rootfs:="n", try_diskmount:="n"
## )
gen_union_mount_script() {
   <%%locals !\
      | phase=${1:?} script_name=${2:?} !\
      | V=${3:?} REL_MP=${4:?}  !\
      | AUTO_FILES=${5-default} !\
      | IS_ROOTFS=${6:-n}       !\
      | TRY_DISKMOUNT=${7:-n}   !\
   %>

   case "${AUTO_FILES}" in
      _|none) AUTO_FILES= ;;
   esac

   set -- \
      <%% foreach_static var !\
         | V REL_MP TRY_DISKMOUNT AUTO_FILES IS_ROOTFS !\
         !\
         | ::: !\
         !\
         |{I}"{var}=${{{var}}}" {BSNL} !\
      %>
      _

   autodie mkdir -p -- "${ISHARE_HOOKS}/union_mount/${script_name}"
   add_env "${V}_HOOKDIR" "${ISHARE_HOOKS}/union_mount/${script_name}"

   gen_from_metascript union_mount "${phase}" "${script_name}" "${@}"
}
