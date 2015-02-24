#!@@XSHELL@@
## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set -u

. "@@FUNCTIONS@@" || exit 9
umask 0022

if [ -z "${PHASE-}" ]; then
   ewarn "PHASE not passed by /init - trying to autodetect."

   case "${0%/*}" in
      early-env-setup|\
      parse-cmdline|\
      env-setup|\
      devfs-setup|\
      net-setup|\
      net-ifup|\
      post-net|\
      pre-mount-newroot|\
      mount-newroot|\
      post-mount-newroot|\
      subtree-mount|\
      populate-newroot|\
      setup-newroot|\
      finalize-newroot|\
      net-ifdown)
         PHASE="${0%/*}"
         ewarn "detected PHASE=${PHASE}"
      ;;

      *)
         die "could not autodetect PHASE."
      ;;
   esac
fi

_find_hooks() {
   local f
   while [ $# -gt 0 ]; do
      <%%foreach f "${1}/"*.sh ::: [ ! -f "${f}" ] || @@ECHO@@ "${f##*/}" %>
      shift
   done | sort -u
}

_RUN_HOOKDIR="@@ISHARE_HOOKS@@/${PHASE}.d"
_CFG_HOOKDIR="${INIT_HOOKDIR:?}/${PHASE}.d"
_HOOKDIR=

# check whether static and/or dynamic hook dir(s) exist
hooks=
if [ -d "${_RUN_HOOKDIR}" ]; then

   if [ -d "${_CFG_HOOKDIR}" ]; then
      hooks="$(_find_hooks "${_RUN_HOOKDIR}" "${_CFG_HOOKDIR}")"
      #_HOOKDIR=

   else
      hooks="$(_find_hooks "${_RUN_HOOKDIR}")"
      _HOOKDIR="${_RUN_HOOKDIR}"
   fi

elif [ -d "${_CFG_HOOKDIR}" ]; then
   hooks="$(_find_hooks "${_CFG_HOOKDIR}")"
   _HOOKDIR="${_CFG_HOOKDIR}"
fi

# exit now if no hooks found
[ -n "${hooks}" ] || exit 0


if [ -n "${_HOOKDIR}" ]; then
get_hookfile() {
   hookfile="${_HOOKDIR}/${1}"
}
else
get_hookfile() {
   hookfile="${_RUN_HOOKDIR}/${1}"
   [ -f "${hookfile}" ] || hookfile="${_CFG_HOOKDIR}/${1}"
}
fi


# run hooks

autodie newroot_remount_setup_flag_files

exit_code=0
for __HOOK__ in ${hooks}; do
   get_hookfile "${__HOOK__}"

   if ! ( loadscript_only "${hookfile}"; ); then
      exit_code=2
      eerror "Failed to run hook ${__HOOK__} (${hookfile})"
      break
   fi
done

autodie newroot_remount_restore_status
exit ${exit_code:-5}
