#!/bin/sh
## Do not load this file directly.
##
##
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

: ${__HAVE_ARCH_BOOTSTRAP_ENV:?}
: ${_ARCH_BOOTSTRAP_SRCDIR:?}
: ${S:?}
: ${B:?}
: ${D:?}

## unpack and move %S/root.<target_arch> to %D
if [ "${FORCE_UNPACK:-X}" = "y" ]; then
   union_mount_get_and_unpack_tarball_forced "${S}" "${bootstrap_uri}"
else
   union_mount_get_and_unpack_tarball "${S}" "${bootstrap_uri}"
fi
autodie test ! -d "${B}"

found_root=
for x in "${S}/root."*; do
   if [ -d "${x}" ]; then
      if [ -n "${found_root}" ]; then
         die "Found more than one bootstrap directory?"
      else
         autodie mv -- "${x}" "${B}"
         found_root="${x}"
      fi
   fi
done
[ -n "${found_root}" ] || die "No bootstrap directory found!"

case "${B}" in
   "${S}/"*)
      true
   ;;
   *)
      fs_remove "${S}"
   ;;
esac


## creates files/dirs
archstrap_set_extra_staging_paths "${D}"
dodir "${ARCHSTRAP_TARGET_OVERLAY}"
dodir "${ARCHSTRAP_PKG_GRPLIST_DIR}"

## build dir
archstrap_set_staging "${B}"

dodir "${pacman_confdir}"

if [ -n "${PACSTRAP_MIRROR}" ]; then
   if [ -f "${pacman_mirrorlist}" ]; then
      autodie mv -f -- "${pacman_mirrorlist}" "${pacman_mirrorlist}.dist"
      copyfile_staging_to_target_overlay "${PACMAN_MIRRORLIST_REL}.dist"
   fi

   {
      printf '%s\n' \
         "Server = http://${PACSTRAP_MIRROR#http://}/archlinux/\$repo/os/\$arch"
   } > "${pacman_mirrorlist}" || die "Failed to create ${pacman_mirrorlist}"

   copyfile_staging_to_target_overlay "${PACMAN_MIRRORLIST_REL}"
fi

# shellcheck disable=SC2015
[ -f "${pacman_mirrorlist}" ] && [ -s "${pacman_mirrorlist}" ] || \
   die "${pacman_mirrorlist} is empty or missing."

## create/edit pacman.conf

##  first, move the original file
autodie mv -f -- "${pacman_conffile}" "${pacman_conffile}.dist"

##  CheckSpace fails, disable it.
autodie sed -r -e 's=^(CheckSpace)=#\1=' -i "${pacman_conffile}.dist"

##  create SigLevel=Never pacman.conf.unsafe
sed -r -e 's,^(SigLevel\s*=\s*).*$,\1Never,' \
   "${pacman_conffile}.dist" > "${pacman_conffile}.unsafe" || \
      die "Failed to create temporary pacman.conf!"

## (sym)link it
autodie ln -s -- "${PACMAN_CONFFILE_REL##*/}.unsafe" "${pacman_conffile}"
autodie test -s "${pacman_conffile}"


if [ -s /etc/resolv.conf ]; then
   autodie copyfile /etc/resolv.conf "${B}/etc/resolv.conf"
else
   { printf '%s\n' 'nameserver 8.8.8.8' > "${B}/etc/resolv.conf"; } || \
      die "Failed to create ${B}/etc/resolv.conf"
fi
copyfile_staging_to_target_overlay /etc/resolv.conf

## move %B => %D (if not equal)
if [ "${B}" != "${D}" ]; then
   autodie movedir_into "${B}" "${D}"
   archstrap_set_staging "${D}"
fi


## chroot(ed) setup - init pacman keys etc.


# dedup package group list (%package_group_list)
for grp_name in \
   "base" ${PACSTRAP_PKG_GRP?} \
   ${ARCHSTRAP_EXPAND_PACKAGE_GRP?}
do
   printf '%s\n' "${grp_name}"
done > "${ARCHSTRAP_PKG_GRPLIST_DIR}/group.list.in" || \
   die "Failed to create intermediate package group list!"

sort -u \
   "${ARCHSTRAP_PKG_GRPLIST_DIR}/group.list.in" \
   > "${ARCHSTRAP_PKG_GRPLIST_DIR}/group.list" || \
   die "Failed to create package group list!"


{
   [ -s "${ARCHSTRAP_PKG_GRPLIST_DIR}/group.list" ] && \
   package_group_list="$( cat "${ARCHSTRAP_PKG_GRPLIST_DIR}/group.list" )" && \
   [ -n "${package_group_list}" ]
} || die "Package group list is empty!"

autodie rm -- "${ARCHSTRAP_PKG_GRPLIST_DIR}/group.list.in"

{
## unfortunately, pacman-key --init hangs
## "forever" on certain systems, e.g. qemu/kvm
##  See also: https://bugs.archlinux.org/task/30286
##  -- using haveged
##

cat << EOF
#!/bin/sh
cd / || exit
export HOME=/root
mkdir -p -m 0700 -- "${HOME}" || exit

HAVEGED_PIDFILE=/tmp/haveged.pid

if [ -f /lib/gentoo/functions.sh ]; then
   . /lib/gentoo/functions.sh || exit
   set -f
else
   set -fu
einfo() { printf '%s %s\n' "(in-chroot)" "\${*}"; }
fi

einfo "Syncing repos"
pacman --noconfirm -Sy @@QUIET@@ || exit

while read -r pkg_grp; do
   einfo "Creating the list of \${pkg_grp} packages"
   pacman --noconfirm -Sg "\${pkg_grp}" > \\
      "${ARCHSTRAP_PACKAGE_GRPLIST_REL%/}/\${pkg_grp}.in" || exit
done < "${ARCHSTRAP_PACKAGE_GRPLIST_REL%/}/group.list" || exit

einfo "Installing busybox"
pacman --noconfirm --needed -S busybox @@QUIET@@ || exit
mkdir -p -- /busybox && /usr/bin/busybox --install -s /busybox || exit
export PATH="\${PATH:+\${PATH}:}/busybox"

einfo "Installing haveged"
pacman --noconfirm --needed -S haveged @@QUIET@@ || exit

einfo "Starting haveged"
rm -f -- "\${HAVEGED_PIDFILE}"
start-stop-daemon -S -p "\${HAVEGED_PIDFILE}" \\
   -x /usr/bin/haveged -- -r 0 -w 1024 -p "\${HAVEGED_PIDFILE}" || exit

einfo "Setting up pacman keys"
fail=
if [ "${ARCHSTRAP_STAGING_NEED_KEY_REFRESH:-X}" = "y" ]; then
{
   mkdir -p -m 0700 /root/.gnupg && \\
   :> /root/.gnupg/dirmngr_ldapservers.conf && \\
   pacman-key --init && \\
   pacman-key --populate archlinux && \\
   pacman-key --refresh-keys && \\
   touch "/HAVE_PACMAN_KEY_SETUP"
}  || fail=\${?}
else
{
   mkdir -p -m 0700 /root/.gnupg && \\
   :> /root/.gnupg/dirmngr_ldapservers.conf && \\
   pacman-key --init && \\
   pacman-key --populate archlinux && \\
   touch "/HAVE_PACMAN_KEY_SETUP"
} @@QUIET@@ || fail=\${?}
fi

einfo "Killing haveged"
start-stop-daemon -K -p "\${HAVEGED_PIDFILE}" -x /usr/bin/haveged && \\
   rm -f -- "\${HAVEGED_PIDFILE}" || : \${fail:=44}


for prog in gpg-agent; do
   einfo "Killing \${prog}"
   ## or killall
   start-stop-daemon -K -x "/usr/bin/\${prog}" || @@NOP@@
done

exit \${fail:-0}
EOF
} > "${STAGING}/chroot_setup.sh" || \
   die "failed to create chroot-setup script!"

autodie chmod 0700 "${STAGING}/chroot_setup.sh"


if [ -f /lib/gentoo/functions.sh ]; then
   dodir "${STAGING}/lib/gentoo"
   autodie copyfile \
      /lib/gentoo/functions.sh "${STAGING}/lib/gentoo/functions.sh"
fi


fs_remove "${STAGING}/HAVE_PACMAN_KEY_SETUP"
archstrap_staging_basemounts

if archstrap_chroot_staging /bin/sh /chroot_setup.sh; then
   [ "${KEEPMNT:-n}" = "y" ] || archstrap_staging_eject_basemounts
else
   archstrap_staging_eject_basemounts
   die "chroot-setup failed (rc=${fail})" ${fail}
fi
autodie test -f "${STAGING}/HAVE_PACMAN_KEY_SETUP"

autodie rm    -- "${STAGING}/chroot_setup.sh"
autodie rm    -- "${pacman_conffile}" "${pacman_conffile}.unsafe"
autodie ln -s -- "${PACMAN_CONFFILE_REL##*/}.dist" "${pacman_conffile}"
autodie test -s "${pacman_conffile}"



## process_pkg_group_infile ( grp_name )
process_pkg_group_infile() {
   : ${1:?}
   <%%locals !\
      | infile=${ARCHSTRAP_PKG_GRPLIST_DIR}/${1}.in !\
      | outfile=${ARCHSTRAP_PKG_GRPLIST_DIR}/${1} !\
   %>

awk -v "grp=${1:?}" \
'
($3 != "")     { exit 2; }
($2 == "")     { exit 3; }
($1 != ""grp)  { exit 4; }
{ print $2; }
' \
"${infile}" > "${outfile}" || die "Failed to create ${1} package list!"

autodie rm -- "${infile}"
}

<%%foreach pkg_grp ${package_group_list} ::: process_pkg_group_infile ${pkg_grp} %>
