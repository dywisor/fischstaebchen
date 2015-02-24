## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

__phaseout_filter() {
   [ "${X_PHASEOUT_LOADSCRIPT_STATUS:-X}" = "main" ] || return 0

   if [ -n "${F_PHASEOUT_HOOK_FILTER-}" ]; then
      ${F_PHASEOUT_HOOK_FILTER} "${__NAME__}" || return ${?}
   fi
}

__phaseout_success() {
   [ "${X_PHASEOUT_LOADSCRIPT_STATUS:-X}" = "main" ] || return 0

   autodie rm -- "${PHASEOUT_HOOK_SYNCDIR_PENDING}/${__NAME__}"
   autodie touch -- "${PHASEOUT_HOOK_SYNCDIR_SUCCESS}/${__NAME__}"
}

__phaseout_setenv_hook() {
   [ "${X_PHASEOUT_LOADSCRIPT_STATUS:-X}" = "main" ] || return 0

   TMPDIR="${__phaseout_tmpdir}"
   T="${__phaseout_tmpdir}"

   case "${PHASEOUT_HOOK_MODE:?}" in
      load)
         @@NOP@@
      ;;
      *)
         export TMPDIR
         export T
      ;;
   esac
}

__phaseout_setenv_pre__common() {
   __phaseout_tmpdir="${PHASEOUT_HOOK_SYNCDIR_GRP}/${grp_name}/${__NAME__}"
   autodie touch -- "${PHASEOUT_HOOK_SYNCDIR_PENDING}/${__NAME__}"

   autodie mkdir -p -- "${__phaseout_tmpdir}"
}

__phaseout_single_setenv_pre() {
   [ "${X_PHASEOUT_LOADSCRIPT_STATUS:-X}" = "main" ] || return 0

   <%% locals grp_name=${__NAME__%%-*} %>

   __phaseout_setenv_pre__common || return

   if [ "${PHASEOUT_HOOK_MODE:-X}" = "load" ]; then
      einfo "Loading ${PHASEOUT_HOOK_NAMESPACE:-???} file: ${__NAME__}"
   else
      einfo "Running ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${__NAME__}"
   fi
}

<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
__phaseout_parallel_setenv_pre() {
   [ "${X_PHASEOUT_LOADSCRIPT_STATUS:-X}" = "main" ] || return 0

   <%% locals grp_name=${__NAME__%%-*} is_new_grp %>

   if mkdir -- "${PHASEOUT_HOOK_SYNCDIR_GRP}/${grp_name}" @@QUIET@@; then
      is_new_grp=y
   else
      is_new_grp=
   fi

   __phaseout_setenv_pre__common || return

   ##PHASEOUT_HOOK_MODE == parallel
   if [ "${grp_name}" != "${__NAME__}" ]; then
      if [ "${is_new_grp:-X}" = "y" ]; then
         einfo "Starting ${PHASEOUT_HOOK_NAMESPACE:-???} hooks: ${grp_name}-*"
      fi

      veinfo "Starting ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${__NAME__}"
   else
      einfo "Starting ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${__NAME__}"
   fi
}
<% endif %>
