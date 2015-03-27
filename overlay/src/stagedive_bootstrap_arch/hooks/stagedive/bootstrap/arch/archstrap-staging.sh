#!/bin/sh
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

loadscript_simple_or_die "${__DIR__}/_functions.sh"
loadscript_simple_or_die "${_ARCH_BOOTSTRAP_SRCDIR:?}/_functions-staging.sh"


archstrap_staging_only__base() {
   union_mount_add_backing_mem_branch ro arch_staging staging zram 512m

   staging_mp="${mp:?}"
   staging_tmp="${staging_mp}/staging.tmp"
   staging="${staging_mp}/bootstrap"

   S="${staging_tmp}"
   B="${staging}"
   D="${staging_mp}"
   KEEPMNT=n
   FORCE_UNPACK=n
   autodie _archstrap_do_run_bootstrap_staging
}

archstrap_staging_only__geninit__print() {
   local default_tty

   default_tty=
   case "${CONSOLETYPE-}" in
      vt)
         ## hacky, but arch ships bootstrap images for x86[_64] only,
         ## so assuming that tty1 is the correct vt is...usually true
         default_tty=/dev/tty1
      ;;
   esac

cat << EOF
#!/usr/bin/sh
[ -r /etc/profile ] && . /etc/profile || :
SHELL=/usr/bin/sh
export DEFAULT_TTY_DEV=${default_tty}
TTY_DEV=\${DEFAULT_TTY_DEV}

: \${PATH:=/usr/bin}
[ ! -d /busybox ] || PATH="\${PATH}:/busybox"
export PATH

while :; do
   if [ -f /TTY ]; then
      if { read -r TTY_DEV < /TTY; } 2>/dev/null; then
         if [ -c "\${TTY_DEV}" ]; then
            rm -f /TTY
         else
            TTY_DEV=
         fi
      else
         TTY_DEV=
      fi
   fi

   if \
      [ -n "\${TTY_DEV}" ] && [ -c "\${TTY_DEV}" ] && \
      [ -x /usr/bin/setsid ]
   then
      /usr/bin/setsid \${SHELL:?} -c \
         "exec \${SHELL:?} <\${TTY_DEV} >\${TTY_DEV} 2>&1"
   else
      \${SHELL:?}
   fi
done
EOF
}

archstrap_staging_only__geninit() {
   archstrap_staging_only__geninit__print > "${D:?}/init" || \
      die "Failed to create /init in ${D:-???}"
   autodie chmod 0755 "${D}/init"
   printf '%s\n' '/init' > "${ISHARE_CFG:?}/init_prog" || \
      die "Failed to register /init!"
}

archstrap_staging_only() {
   autodie archstrap_staging_only__base
   autodie archstrap_staging_only__geninit
}
