#!/bin/sh
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

loadscript_or_die "${__DIR__}/_functions.sh"
. "${_ARCH_BOOTSTRAP_SRCDIR:?}/_functions-staging.sh" || die "#fload target"
. "${_ARCH_BOOTSTRAP_SRCDIR:?}/_functions-target.sh"  || die "#fload staging"


## .bootstrap files usually need to call this function only
## archstrap_default ( **ARCHSTRAP_PROFILES )
archstrap_default() {
   <%%locals ROOT %>

   stagedive_bootstrap_default_init \
      "Initializing an Arch Linux system [${STAGEDIVE_TYPE#archstrap-}]"

   [ -n "${ARCHSTRAP_PROFILES-}" ] || \
      die "archstrap_default(): ARCHSTRAP_PROFILES is not set."

   set -- ${ARCHSTRAP_PROFILES}
   [ ${#} -gt 0 ] || die "archstrap_default(): profile list is empty."

   # read profile files and create runtime profile dir
   archstrap_setup_install_profile "${@}"

   # set up the staging area
   archstrap_staging

   # bootstrap target
   archstrap_target

   if [ "${ARCHSTRAP_DOTFILES_AFTER_PKG:-X}" = "y" ]; then
      # install additional packages
      archstrap_target_install_packages

      # dotfiles
      _archstrap_target_install_dotfiles
   else
      # dotfiles
      _archstrap_target_install_dotfiles

      # install additional packages
      archstrap_target_install_packages
   fi

   # ... done!
   archstrap_done
}

_archstrap_target_install_dotfiles() {
   # dotfiles
   if ishare_has_flag want-dotfiles-import; then
      ROOT="${TARGET}"

      root_import_dotfiles
      ishare_del_flag want-dotfiles-import

      root_install_system_dotfiles
      add_env STAGEDIVE_INSTALL_SYSTEM_DOTFILES n
   fi
}

## archstrap_setup_install_profile ( [*profile_optional], profile )
archstrap_setup_install_profile() {
   <%%locals pfile_names %>
   <%%locals k phase_iter profile_iter basepath f d found_any_file %>
   <%%locals pkg_list grp_list grp_expandonly_list %>

   pfile_names="pkg grp grp_expandonly"

   # create profile dir root
   dodir "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}"
   :> "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all" || die "grp.all"

   [ -n "${ARCHSTRAP_TARGET_INSTALL_PHASES?}" ] || return 0

   :> "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all.in" || die "grp.all.in"
   for phase_iter in ${ARCHSTRAP_TARGET_INSTALL_PHASES}; do
      d="${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR}/${phase_iter}"

      dodir "${d}"
      for k in ${pfile_names:?}; do
         :> "${d}/${k}"    || die "Failed to create ${d}/${k}"
         :> "${d}/${k}.in" || die "Failed to create ${d}/${k}.in"
      done
   done

   k=0
   for profile_iter; do
      <%% inc k %>
      basepath="${_ARCH_BOOTSTRAP_SRCDIR:?}/pkglist/${profile_iter}"
      found_any_file=

      if [ -f "${basepath}.filter" ]; then
         autodie read_text_file_do \
            "${basepath}.filter" _archstrap_append_to_pkg_filter
         [ ${k} -eq ${#} ] || found_any_file=filter
      fi

      for phase_iter in ${ARCHSTRAP_TARGET_INSTALL_PHASES}; do
         pkg_list=
         grp_list=
         grp_expandonly_list=

         f="${basepath}.${phase_iter}"

         if [ -f "${f}" ]; then
            autodie __archstrap_read_pkglist_file "${f}"
            found_any_file=pkg

            d="${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR}/${phase_iter}"
            {
               printf '%s\n' "${pkg_list}"            >> "${d}/pkg.in"  && \
               printf '%s\n' "${grp_list}"            >> "${d}/grp.in"  && \
               printf '%s\n' "${grp_expandonly_list}" >> "${d}/grp_expandonly.in"
            } || die "Failed to write profile data!"
         fi
      done

      if [ -n "${found_any_file}" ]; then
         @@NOP@@
      elif [ ${k} -eq ${#} ]; then
         die "profile empty or not found: ${profile_iter}"
      else
         ewarn "profile empty or not found: ${profile_iter}"
      fi
   done

   for phase_iter in ${ARCHSTRAP_TARGET_INSTALL_PHASES}; do
      d="${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR}/${phase_iter}"

      for k in ${pfile_names:?}; do
         f="${d}/${k}"
         ## do not use "set -o pipefail"
         < "${f}.in" xargs -r printf '%s\n' | grep -- . | sort -u  > "${f}" || \
            die "Failed to process ${f}.in"

         autodie rm -- "${f}.in"
      done

      for k in grp grp_expandonly; do
         cat "${d}/${k}" \
            >> "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all.in" || die "grp.all.in"
      done

      sort -u "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all.in" \
         > "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all" || die "gen grp.all"

      autodie rm -- "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all.in"

      grp_expandonly_list="$( \
         cat "${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/grp.all" )" || \
            die "#read grp_expandonly_list"

      ARCHSTRAP_EXPAND_PACKAGE_GRP="${ARCHSTRAP_EXPAND_PACKAGE_GRP} ${grp_expandonly_list}"
   done

   return 0
}

archstrap_target_install_packages() {
   <%%varcheck ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR %>
   <%%local phase_iter %>
   [ -n "${TARGET-}" ] || \
      die "Cannot install packages to target: need to set up target first."

   [ -n "${ARCHSTRAP_TARGET_INSTALL_PHASES?}" ] || return 0

   for phase_iter in ${ARCHSTRAP_TARGET_INSTALL_PHASES?}; do
      autodie _archstrap_target_install_packages_from_profile "${phase_iter}"
   done
}

_archstrap_target_install_packages_from_profile() {
   <%%local fail d phase grp_list pkg_list pkg_install_list %>
   phase="${1:?}"
   d="${ARCH_BOOTSTRAP_INSTALL_PROFILE_DIR:?}/${phase}"
   set --

   if [ ! -d "${d}" ]; then
      eerror "Missing install profile dir for ${phase} (${d})"
      return 2
   fi

   grp_list="$(cat "${d}/grp")" || die "#fread grp_list"
   pkg_list="$(cat "${d}/pkg")" || die "#fread pkg_list"

   if [ -z "${grp_list}${pkg_list}" ]; then
      einfo "Nothing to install for the '${phase}' phase."
      return 0
   fi

   autodie _archstrap_get_package_install_list \
      "-" "${grp_list}" -- "${pkg_list}"

   set -f; set -- ${pkg_install_list}; set +f
   if [ ${#} -eq 0 ]; then
      ewarn "Nothing to install for the '${phase}' phase: everything filtered out!"
      return 0
   fi

   einfo "Installing ${phase} packages (${#} packages in total):"

   if [ "${TARGET_HAVE_BASEMOUNTS:-X}" = "y" ]; then
      archstrap_target_zap_pkgdir
   else
      archstrap_target_basemounts
   fi

   # the --quiet switch doesn't seem to have any effect
   if [ "${ARCHSTRAP_TARGET_PACINS_QUIET:-X}" = "y" ]; then
      einfo "... quiet install"
      @@QUIET@@ _archstrap_chroot_target \
         pacman --noconfirm \
            -S --quiet --needed ${ARCHSTRAP_TARGET_PACINS_OPTS-} "${@}"
   else
      _archstrap_chroot_target \
         pacman --noconfirm \
            -S --quiet --needed ${ARCHSTRAP_TARGET_PACINS_OPTS-} "${@}"
   fi

   if [ ${fail} -eq 0 ]; then
      einfo "${phase} installation complete."
   else
      eerror "Failed to install ${phase} packages."
      return ${fail}
   fi
}

archstrap_staging() {
   [ -z "${STAGING-}" ] || die "Staging is already set up!"

   <%%locals mp %>
   #non-locals: staging_mp staging staging_tmp S B D KEEPMNT FORCE_UNPACK

   if [ "${ARCHSTRAP_KEEP_STAGING:-X}" = "y" ]; then
      union_mount_add_backing_mem_branch_detached arch_staging staging zram 512m
      staging_mp="${mp:?}"
      FORCE_UNPACK=n
   else
      staging_mp=/mnt/archstrap_staging
      xmount "${staging_mp}" arch_staging tmpfs rw,mode=0755,size=1000m
      FORCE_UNPACK=y
   fi

   staging_tmp="${staging_mp}/staging.tmp"
   staging="${staging_mp}/bootstrap"

   S="${staging_tmp}"
   B="${staging}"
   D="${staging}"
   KEEPMNT=y
   autodie _archstrap_do_run_bootstrap_staging

   archstrap_set_staging "${staging}"
   archstrap_set_extra_staging_paths
}

archstrap_target() {
   [ -z "${TARGET-}"  ] || die "Target is already set up!"
   [ -n "${STAGING-}" ] || die "Cannot set up target: needs staging."
   : ${ARCHSTRAP_TARGET_OVERLAY:?}
   : ${ARCHSTRAP_PKG_GRPLIST_DIR:?}

   <%%locals fail d %>
   #non-locals: target TARGET pacstrap_pkg_list!

   get_pacstrap_pkg_list || die "pacstrap package list is empty!"

   union_mount_add_backing_mem_branch ro arch_target target \
      "${ARCHSTRAP_TARGET_MEM_TYPE:?}" "${ARCHSTRAP_TARGET_MEM_SIZE:?}"
   target="${mp:?}"

   xmount_bind "${target}" "${STAGING}/mnt/target"

   einfo "Running pacstrap"
   if archstrap_chroot_staging \
      pacstrap /mnt/target ${pacstrap_pkg_list:?} @@NO_STDOUT@@
   then
      archstrap_staging_eject_basemounts
   else
      archstrap_staging_eject_basemounts
      die "Failed to bootstrap target!" ${fail}
   fi


   # the default resolv.conf is just useless
   fs_remove "${target}/etc/resolv.conf"

   autodie copytree "${ARCHSTRAP_TARGET_OVERLAY:?}" "${target}"

   if [ "${ARCHSTRAP_KEEP_STAGING:-X}" = "y" ]; then
      autodie _union_mount_add_branch ro "${staging_mp}"

   else
      ## mv -T
      d="${target}/${ARCHSTRAP_TARGET_OVERLAY_REL#/}"
      dodir "${d%/*}"
      autodie mv -- "${ARCHSTRAP_TARGET_OVERLAY}" "${d}"
      ARCHSTRAP_TARGET_OVERLAY="${d}"

      d="${target}/${ARCHSTRAP_PACKAGE_GRPLIST_REL#/}"
      dodir "${d%/*}"
      autodie mv -- "${ARCHSTRAP_PKG_GRPLIST_DIR}" "${d}"
      ARCHSTRAP_PKG_GRPLIST_DIR="${d}"

      autodie umount "${staging_mp}"
   fi

   # as of Feb 2015, installation fails *randomly*
   # if the following files/dirs do not exist in %target:
   ## (_probably_ depends on the list of packages to be installed)
   autodie mkdir -p -m 0700 -- "${target}/root"
   autodie mkdir -p -m 0700 -- "${target}/root/.gnupg"
   :> "${target}/root/.gnupg/dirmngr_ldapservers.conf" || die "#.gnupg"

   archstrap_set_target "${target}"
}

archstrap_done() {
   archstrap_target_eject_basemounts_if_mounted
}
