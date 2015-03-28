## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## stagedive_want ( ["--"|"-r"|"-R"|"--recursive"], *name )
stagedive_want() {
   # shellcheck disable=SC2031
   _phaseout_install_local "${STAGEDIVE_HOOKS_SRCDIR}" "${@}"
}

## stagedive_want_as ( name, as_name )
stagedive_want_as() {
   # shellcheck disable=SC2031
   _phaseout_install_local_as "${STAGEDIVE_HOOKS_SRCDIR}" "${@}"
}

stagedive_inherit() {
   : ${__stagedive_tro?}
   <%%locals stage_cfgfile=${STAGEDIVE_PROFILES_DIR}/${1}.sh %>

   if list_has "${1}" ${__stagedive_tro}; then
      return 0

   elif [ -f "${stage_cfgfile}" ]; then
      __stagedive_tro="${__stagedive_tro} ${1}"
      loadscript_simple_or_die "${stage_cfgfile}"

   else
      die "missing stagedive profile file: ${1} (${stage_cfgfile})"
   fi
}

## @autodie _stagedive_run_setup_hooks (...)
##
_stagedive_run_setup_hooks() {
   # shellcheck disable=SC2031
   ( __stagedive_run_setup_hooks "${@}"; ) || \
      die "Failed to run stagedive setup hooks!"
}

## @need-subshell __stagedive_run_setup_hooks (
##    profile, root, hook_mask:="none"
## )
##
__stagedive_run_setup_hooks() {
   <%%locals __stagedive_tro have_any_hook %>
   <%%locals profile=${1:?} ROOT=${2:?} hook_mask=${3:-none} %>

   ## valid profile name?
   case "${profile}" in
      none)
         die "__stagedive_run_setup_hooks(): profile must not be ${profile}"
      ;;
   esac

   ## ROOT != NEWROOT not implemented so far
   ##  (possible solution: local REAL_NEWROOT=%NEWROOT; local NEWROOT=$ROOT)
   ##
   if [ "${ROOT}" != "${NEWROOT}" ]; then
      die "__stagedive_run_setup_hooks(): ROOT != NEWROOT not supported so far."
   fi

   ## catch hook_mask=="all" prior to the "profile exists" check
   if [ "${hook_mask}" = "all" ]; then
      ewarn "Skipping stagedive-setup-${profile}: all hooks disabled."
   fi

   ## profile exists?
   if [ ! -f "${STAGEDIVE_PROFILES_DIR}/${profile}.sh" ]; then
      die "stagedive type not implemented: ${profile}"
   fi

   ## localize %STAGEDIVE_TYPE if != %profile
   if [ "${profile}" != "${STAGEDIVE_TYPE:-none}" ]; then
      local STAGEDIVE_TYPE
      STAGEDIVE_TYPE="${profile}"
   fi

   ## run hooks
   autodie phaseout_init "stagedive-setup-${STAGEDIVE_TYPE}"

   loadscript_simple_if_exists_or_die "${STAGEDIVE_CONFIG_ROOT}/env.sh"

   __stagedive_tro=
   stagedive_inherit "${STAGEDIVE_TYPE}"
   veinfo "stagedive tro: ${__stagedive_tro# }"

   phaseout_apply_mask "${hook_mask}"
   if [ -z "${have_any_hook?}" ]; then
      einfo "${STAGEDIVE_TYPE}: no hooks found!"
      autodie phaseout_finalize
      return 0
   fi

   autodie phaseout_run_default

   if [ "${STAGEDIVE_CLEANUP_HOOKDIR:-y}" = "y" ]; then
      autodie phaseout_finalize
   fi
}
