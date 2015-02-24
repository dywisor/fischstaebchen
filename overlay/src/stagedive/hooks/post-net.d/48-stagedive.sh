## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

[ "${STAGEDIVE_TYPE:-none}" != "none" ] || return 0

case "${STAGEDIVE_TYPE:-none}" in
   none)
      return 0
   ;;

   gentoo-stage3|archstrap*)
      # FIXME: not here
      ishare_add_flag stagedive-want-create-fstab
   ;;
esac

[ "${STAGEDIVE_AUTOSET_FILE_CFG:-none}" != "none" ] || return 0



if ! ishare_has_flag have-net; then
   die "stagedive:${STAGEDIVE_TYPE} autoset needs networking!"
fi

if [ -z "${VOLATILE_ROOTFS_HOOKDIR-}" ]; then
   die "volatile_rootfs is not configured?"
fi
STAGEDIVE_BOOTSTRAP_HOOK="${VOLATILE_ROOTFS_HOOKDIR}/stagedive_bootstrap.sh"

# by default, create /etc/fstab in %NEWROOT
#  no-op if flag file already created
ishare_add_flag stagedive-want-create-fstab

bootstrap_cfg_from_command() {
   local fail

   if "${@}" > "${STAGEDIVE_BOOTSTRAP_CFG}"; then
      if [ -s "${STAGEDIVE_BOOTSTRAP_CFG}" ]; then
         return 0
      else
         eerror "Failed to get non-empty bootstrap config for ${STAGEDIVE_TYPE}!"
      fi
   else
      fail=${?}
      eerror "Failed to get ${STAGEDIVE_TYPE} bootstrap config!"
      eerror "'${*}' returned ${fail}"
   fi

   die "Failed to get bootstrap config." ${fail}
}

bootstrap_cfg_from_uri_getter() {
   : ${1:?}

   if [ -z "${APT_PROXY-}" ]; then
<% if OFFENSIVE= %>
      ewarn "stagedive-bootstrap: apt-cacher-ng not configured!"
<% else %>
      die "stagedive-bootstrap: apt-cacher-ng not configured!"
<% endif %>
   fi

   export VPREFIX=bootstrap_
   bootstrap_cfg_from_command "${@}" -x \
      "${STAGEDIVE_AUTOSET_FILE_CFG}" - "${APT_PROXY}"
}

try_install_bootstrap_hook() {
   <%% locals v0 v1 F_IMPORT_FILE_INTERCEPT %>

   case "${1:?}" in
      "${STAGEDIVE_BOOTSTRAP_SRCDIR}/"*)
         autodie ln -s -- "${1}" "${STAGEDIVE_BOOTSTRAP_HOOK}"
      ;;

      *)
         F_IMPORT_FILE_INTERCEPT=intercept_file_import_pre_dosym
         ## STAGEDIVE_BOOTSTRAP_HOOK contains >1 "/" char
         import_file \
            "${STAGEDIVE_BOOTSTRAP_HOOK%/*}" \
            "${1:?}" \
            "${STAGEDIVE_BOOTSTRAP_HOOK##*/}"
      ;;
   esac
}

install_bootstrap_hook() {
   try_install_bootstrap_hook "${@}" || \
      die "Failed to install stagedive bootstrap hook '${1}'"
}

## create empty bootstrap cfg file
dodir "${STAGEDIVE_RUNTIME_CONFDIR}"
:> "${STAGEDIVE_BOOTSTRAP_CFG}" || \
   die "Failed to initialize stagedvie bootstrap config file!"

## init bootstrap hook (dodir && rm)
dodir_for_file "${STAGEDIVE_BOOTSTRAP_HOOK}"
fs_remove "${STAGEDIVE_BOOTSTRAP_HOOK}"

## locate and load type-specific setup script
f="${STAGEDIVE_BOOTSTRAP_SRCDIR}/${STAGEDIVE_TYPE}.setup"
if [ ! -f "${f}" ]; then
   f="${STAGEDIVE_BOOTSTRAP_SRCDIR}/${STAGEDIVE_TYPE%%-*}-all.setup"

   if [ ! -f "${f}" ]; then
      die "stagedive autoset not implemented for type ${STAGEDIVE_TYPE}!"
   fi
fi
loadscript_or_die "${f}"

## check whether setup is sufficient
[ -f "${STAGEDIVE_BOOTSTRAP_CFG}" ] && \
[ -s "${STAGEDIVE_BOOTSTRAP_CFG}" ] || die "bootstrap config file is empty!"

## install default hook if available
if [ ! -f "${STAGEDIVE_BOOTSTRAP_HOOK}" ]; then
   for f in \
      "${STAGEDIVE_BOOTSTRAP_SRCDIR}/${STAGEDIVE_TYPE}.bootstrap"
   do
      if [ -f "${f}" ]; then
         install_bootstrap_hook "${f}"
         break
      fi
   done
fi

[ -f "${STAGEDIVE_BOOTSTRAP_HOOK}" ] && \
[ -s "${STAGEDIVE_BOOTSTRAP_HOOK}" ] || die "did not set up bootstrap hook!"
