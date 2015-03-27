## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @funcdef usr_rootfs_do_import_<variant> ( src, dst, relsrc )
<%define _MSGPRE usr-rootfs-import %>

# check whether usr-rootfs-import is enabled
case "${USR_ROOTFS_METHOD-}" in
   ''|none)
      veinfo "skipping @@_MSGPRE@@: disabled."
      exit 0
   ;;

   auto)
      einfo "@@_MSGPRE@@: using variant=copy"
      USR_ROOTFS_METHOD=copy
   ;;
esac

# check essential vars
##  (but USR_ROOTFS_METHOD as that gets validated by import_to_dir()
##   anyway)
if \
   [ -z "${NEWROOT_USR_ROOTFS-}" ] || \
   [ -z "${NEWROOT_USR_ROOTFS_REL-}" ]
then
   die "@@_MSGPRE@@: NEWROOT_USR_ROOTFS[_REL] is not set."
fi

# exit now if src dir not present
if [ ! -d "${NEWROOT_USR_ROOTFS}" ]; then
   veinfo \
      "skipping @@_MSGPRE@@: ${NEWROOT_USR_ROOTFS} does not exist."

   exit 0
fi

## ~int usr_rootfs_import_check_set_flag_do ( flag_name, *cmdv )
##
usr_rootfs_import_check_set_flag_do() {
   <%%locals flag_name< %>

   if ishare_has_flag "have-usr-rootfs-import-${flag_name}"; then
      veinfo "@@_MSGPRE@@: ${flag_name} already imported."
      return 0
   fi

   einfo "@@_MSGPRE@@: importing ${flag_name}"
   "${@}" || return
   veinfo "@@_MSGPRE@@: successfully imported ${flag_name}"
   autodie ishare_set_flag "have-usr-rootfs-import-${flag_name}"
}

## int _usr_rootfs_check_did_import_dir ( dirpath )
_usr_rootfs_check_did_import_dir() {
   if test_fs_lexists "${1}/.did_usr_rootfs_import"; then
      return 0
   elif test_fs_lexists "${1}/.did_sfsroot_import"; then
      ewarn "Found deprecated did_sfsroot_import flagfile in ${1}!"
      return 0
   fi

   return 1
}

## @autodie usr_rootfs_do_import_dir ( src, **usr_rootfs_import_variant )
usr_rootfs_do_import_dir() {
   <%%locals src_name="${1##*/}" %>

   if _usr_rootfs_check_did_import_dir "${NEWROOT}/${src_name}"; then
      einfo "@@_MSGPRE@@: skipping ${src_name} - already imported (dstdir flagfile)"
      return 0
   fi

   usr_rootfs_import_check_set_flag_do "${src_name}" \
      import_to_dir \
         "${usr_rootfs_import_variant:?}" \
         "${NEWROOT_USR_ROOTFS_REL}/${src_name}" \
         "${1}" \
         "${NEWROOT}/${src_name}" \
         "${src_name}" || return

   case "${usr_rootfs_import_variant}" in
      cp|copy)
         autodie touch "${NEWROOT}/${src_name}/.did_usr_rootfs_import"
      ;;
   esac
}

## @autodie usr_rootfs_do_import_symlink ( src )
usr_rootfs_do_import_symlink() {
   <%%locals src_name="${1##*/}" %>

   usr_rootfs_import_check_set_flag_do "${src_name}" \
      copy_symlink "${1}" "${NEWROOT}/${src_name}" "${src_name}"
}

## @autodie _usr_rootfs_glob_do_import_any ( src, **usr_rootfs_import_variant )
_usr_rootfs_glob_do_import_any() {
   ## note that '! -e' does not imply '! -h'
   if test -h "${1}"; then
      usr_rootfs_do_import_symlink "${1}"
   elif test -d "${1}"; then
      usr_rootfs_do_import_dir "${1}"
   fi
}

## @autodie import_all_from_usr_rootfs_dir()
import_all_from_usr_rootfs_dir() {
   <%% locals usr_rootfs_import_variant src_iter %>
   usr_rootfs_import_variant="${USR_ROOTFS_METHOD:?}"

   <%%foreach src_iter "${NEWROOT_USR_ROOTFS}/"* +++ !\
      | _usr_rootfs_glob_do_import_any "${src_iter}" %>
}

## @autodie _import_from_usr_rootfs_process_file_item (
##    [source_file], **usr_rootfs_import_variant, **arg
## )
_import_from_usr_rootfs_process_file_item() {
   <%%locals v0 %>

   case "${arg}" in
      @@CASE_RELPATH_PARENT@@)
         if [ -n "${1-}" ]; then
            die "in file ${1}: bad src arg: ${arg}" || return
         else
            die "bad src arg: ${arg}" || return
         fi
      ;;
   esac

   v0="${NEWROOT_USR_ROOTFS}/${arg}"

   case "${usr_rootfs_import_variant}" in
      tar|untar)
         [ -f "${v0}" ] || locate_tarball_file "${v0}" || return
         # locate_tarball_file() modifies %v0
         usr_rootfs_do_import_dir "${v0}" || return
      ;;

      cp|copy)
         if test -h "${v0}"; then
            usr_rootfs_do_import_symlink "${v0}" || return
         else
            usr_rootfs_do_import_dir "${v0}" || return
         fi
      ;;

      *)
         usr_rootfs_do_import_dir "${v0}" || return
      ;;
   esac
}

## @autodie import_from_usr_rootfs_readfile ( infile )
import_from_usr_rootfs_readfile() {
   <%%locals usr_rootfs_import_variant arg %>

   while read -r usr_rootfs_import_variant arg; do
      case "${usr_rootfs_import_variant}" in
         ''|'#'*)
            @@NOP@@
         ;;
         *)
            autodie _import_from_usr_rootfs_process_file_item "${1}" || return
      esac
   done < "${1}"
}


get_newroot_rw || exit

if [ -f "${NEWROOT_USR_ROOTFS}/setup.list" ]; then
   import_from_usr_rootfs_readfile \
      "${NEWROOT_USR_ROOTFS}/setup.list"
else
   import_all_from_usr_rootfs_dir
fi

<% if ALLOW_NEWROOT_HOOKS %>
if [ -f "${NEWROOT_USR_ROOTFS}/setup.sh" ]; then
   ( . "${NEWROOT_USR_ROOTFS}/setup.sh"; ) || \
      die "@@_MSGPRE@@: setup.sh failed!"
fi
<% endif %>
