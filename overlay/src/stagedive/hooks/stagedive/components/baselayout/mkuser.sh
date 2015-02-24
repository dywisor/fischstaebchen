## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if ! ishare_has_flag stagedive-bootstrap-want-create-user; then
   ewarn "mkuser not enabled!"
   return 0
fi

: ${STAGEDIVE_MKUSER_NAME:?}
: ${STAGEDIVE_MKUSER_PASS:?}
: ${STAGEDIVE_MKUSER_GROUPS?}
##: ${STAGEDIVE_MKUSER_HOME:=}


group_list="$( \
   printf '%s' "${STAGEDIVE_MKUSER_GROUPS}" | \
      awk 'BEGIN { RS="(,|\\s|\n)"; } { print; }' )"

for grp in ${group_list:?}; do
   case "${grp}" in
      autologin)
         einfo "Creating system group ${grp}"
         grep -- "^${grp}:" "${ROOT%/}/etc/group" @@QUIET@@ || \
            chroot_root groupadd -r "${grp}"
      ;;
   esac
done

set -- useradd -m -U -G "${STAGEDIVE_MKUSER_GROUPS}"
[ -z "${STAGEDIVE_MKUSER_HOME-}" ] || \
   set -- "${@}" -d "${STAGEDIVE_MKUSER_HOME}"
set -- "${@}" "${STAGEDIVE_MKUSER_NAME}"

einfo "Creating live user ${STAGEDIVE_MKUSER_NAME}"
chroot_root "${@}"

einfo "Setting password for user ${STAGEDIVE_MKUSER_NAME}"
root_passwd "${STAGEDIVE_MKUSER_NAME}" "${STAGEDIVE_MKUSER_PASS}"

if [ "${STAGEDIVE_MKUSER_IMPORT_DOTFILES:-y}" = "y" ]; then
   root_install_user_dotfiles "@${STAGEDIVE_MKUSER_NAME}"
fi

if qwhich create-user-tmpdirs; then
   einfo "(Re-)Creating user tmpdirs"
   autodie create-user-tmpdirs "${ROOT%/}/etc/passwd" "${ROOT%/}/tmp/users"
fi

__mkuser_write_sudo_config() {
   einfo "Creating sudo config for ${STAGEDIVE_MKUSER_NAME}"

   autodie mkdir -p -m 0750 -- "${ROOT%/}/etc/sudoers.d"
   write_to_file \
      "${ROOT%/}/etc/sudoers.d/mkuser_${STAGEDIVE_MKUSER_NAME}" \
      "${STAGEDIVE_MKUSER_NAME} ALL=(root) ${*}"
}

case "${STAGEDIVE_MKUSER_SETUP_SUDO:-none}" in
   n|none)
      @@NOP@@
   ;;

   nopasswd|NOPASSWD)
      __mkuser_write_sudo_config "NOPASSWD: ALL"
   ;;

   y|passwd|PASSWD)
      __mkuser_write_sudo_config "PASSWD: ALL"
   ;;

   *)
      die "mkuser: invalid sudo config: ${STAGEDIVE_MKUSER_SETUP_SUDO}"
   ;;
esac
