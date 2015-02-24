## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @funcdef squashed_rootfs_do_import_<variant> ( src, dst, relsrc )
<%define _MSGPRE sfsroot-import %>

# check whether sfsroot-import is enabled
case "${SQUASHED_ROOTFS_METHOD-}" in
   ''|none)
      veinfo "skipping @@_MSGPRE@@: disabled."
      exit 0
   ;;

   auto)
      einfo "@@_MSGPRE@@: using variant=copy"
      SQUASHED_ROOTFS_METHOD=copy
   ;;
esac

# check essential vars
##  (but SQUASHED_ROOTFS_METHOD as that gets validated by import_to_dir()
##   anyway)
if \
   [ -z "${NEWROOT_SQUASHED_ROOTFS-}" ] || \
   [ -z "${NEWROOT_SQUASHED_ROOTFS_REL-}" ]
then
   die "@@_MSGPRE@@: NEWROOT_SQUASHED_ROOTFS[_REL] is not set."
fi

# exit now if src dir not present
if [ ! -d "${NEWROOT_SQUASHED_ROOTFS}" ]; then
   veinfo \
      "skipping @@_MSGPRE@@: ${NEWROOT_SQUASHED_ROOTFS} does not exist."

   exit 0
fi

## ~int sfsroot_import_check_set_flag_do ( flag_name, *cmdv )
##
sfsroot_import_check_set_flag_do() {
   <%%locals flag_name< %>

   if ishare_has_flag "have-sfsroot-import-${flag_name}"; then
      veinfo "@@_MSGPRE@@: ${flag_name} already imported."
      return 0
   fi

   einfo "@@_MSGPRE@@: importing ${flag_name}"
   "${@}" || return
   veinfo "@@_MSGPRE@@: successfully imported ${flag_name}"
   autodie ishare_set_flag "have-sfsroot-import-${flag_name}"
}

## @autodie sfsroot_do_import_dir ( src, **sfsroot_import_variant )
sfsroot_do_import_dir() {
   <%%locals src_name="${1##*/}" %>

   if [ -e "${NEWROOT}/${src_name}/.did_sfsroot_import" ]; then
      einfo "@@_MSGPRE@@: skipping ${src_name} - already imported (dstdir flagfile)"
      return 0
   fi

   sfsroot_import_check_set_flag_do "${src_name}" \
      import_to_dir \
         "${sfsroot_import_variant:?}" \
         "${NEWROOT_SQUASHED_ROOTFS_REL}/${src_name}" \
         "${1}" \
         "${NEWROOT}/${src_name}" \
         "${src_name}" || return

   case "${sfsroot_import_variant}" in
      cp|copy)
         autodie touch "${NEWROOT}/${src_name}/.did_sfsroot_import"
      ;;
   esac
}

## @autodie sfsroot_do_import_symlink ( src )
sfsroot_do_import_symlink() {
   <%%locals src_name="${1##*/}" %>

   sfsroot_import_check_set_flag_do "${src_name}" \
      copy_symlink "${1}" "${NEWROOT}/${src_name}" "${src_name}"
}

## @autodie sfsroot_do_import_any ( src, **sfsroot_import_variant )
sfsroot_do_import_any() {
   ## note that '! -e' does not imply '! -h'
   if test -h "${1}"; then
      sfsroot_do_import_symlink "${1}"
   elif test -d "${1}"; then
      sfsroot_do_import_dir "${1}"
   fi
}

## @autodie import_all_from_squashed_rootfs_dir()
import_all_from_squashed_rootfs_dir() {
   <%% locals sfsroot_import_variant src_iter %>
   sfsroot_import_variant="${SQUASHED_ROOTFS_METHOD:?}"

   <%%foreach src_iter "${NEWROOT_SQUASHED_ROOTFS}/"* +++ !\
      | sfsroot_do_import_any "${src_iter}" %>
}

## @autodie _import_from_squashed_rootfs_process_file_item (
##    **sfsroot_import_variant, **arg
## )
_import_from_squashed_rootfs_process_file_item() {
   <%%locals v0 %>

   case "${arg}" in
      '../'*|*'/..'|*'/../'*)
         die "in file ${1}: bad src arg: ${arg}" || return
      ;;
   esac

   v0="${NEWROOT_SQUASHED_ROOTFS}/${arg}"

   case "${sfsroot_import_variant}" in
      tar|untar)
         [ -f "${v0}" ] || locate_tarball_file "${v0}" || return
         sfsroot_do_import_dir "${v0}" || return
      ;;

      cp|copy)
         if test -h "${v0}"; then
            sfsroot_do_import_symlink "${v0}" || return
         else
            sfsroot_do_import_dir "${v0}" || return
         fi
      ;;

      *)
         sfsroot_do_import_dir "${v0}" || return
      ;;
   esac
}

## @autodie import_from_squashed_rootfs_readfile ( infile )
import_from_squashed_rootfs_readfile() {
   <%%locals sfsroot_import_variant arg %>

   while read -r sfsroot_import_variant arg; do
      case "${sfsroot_import_variant}" in
         ''|'#'*)
            @@NOP@@
         ;;
         *)
            autodie _import_from_squashed_rootfs_process_file_item || return
      esac
   done < "${1}"
}


get_newroot_rw || exit

if [ -f "${NEWROOT_SQUASHED_ROOTFS}/setup.list" ]; then
   import_from_squashed_rootfs_readfile \
      "${NEWROOT_SQUASHED_ROOTFS}/setup.list"
else
   import_all_from_squashed_rootfs_dir
fi

<% if ALLOW_NEWROOT_HOOKS %>
if [ -f "${NEWROOT_SQUASHED_ROOTFS}/setup.sh" ]; then
   ( . "${NEWROOT_SQUASHED_ROOTFS}/setup.sh"; ) || \
      die "@@_MSGPRE@@: setup.sh failed!"
fi
<% endif %>
