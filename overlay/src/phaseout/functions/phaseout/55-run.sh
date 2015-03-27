## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

phaseout_run() {
   _phaseout_run "${@}" && \
   _phaseout_waitfor_hooks && \
   _phaseout_get_and_report_failed
}

phaseout_run_or_die() {
   phaseout_run "${@}" || \
      die "Failed to run ${PHASEOUT_HOOK_NAMESPACE:-???} hooks!"
}

phaseout_run_if_exist() {
   phaseout_run -e "${@}"
}

phaseout_run_if_exist_or_die() {
   phaseout_run_or_die -e "${@}"
}

_phaseout_run_from_hookdir() {
   : ${1:?}
   phaseout_run_if_exist_or_die \
      "${PHASEOUT_HOOK_DIR}/${1}.sh" \
      "${PHASEOUT_HOOK_DIR}/${1}-"*".sh" \
      "${PHASEOUT_HOOK_DIR}/"*"-${1}.sh"
}

_phaseout_run_all_from_hookdir() {
   : ${F_PHASEOUT_HOOK_FILTER:?}
   phaseout_run_if_exist_or_die "${PHASEOUT_HOOK_DIR}/"*".sh"
}

## FUTURE: create per-phase directories in PHASEOUT_HOOK_DIR/
##  add whole-group-{shared,subshell-detached,shared-detached}
_phaseout_run_default_filter_env() {
   case "${__NAME__}" in
      env|env-*|*-env)
         return 1
      ;;
   esac
}

_phaseout_run_default_filter_non_parallel() {
   case "${__NAME__}" in
      baselayout|baselayout-*|*-baselayout|\
      env|env-*|*-env|\
      fini|fini-*|*-fini)
         return 1
      ;;
   esac
}

_phaseout_run_default_filter_env_baselayout() {
   case "${__NAME__}" in
      baselayout|baselayout-*|*-baselayout|\
      env|env-*|*-env)
         return 1
      ;;
   esac
}

phaseout_run_default() {
   phaseout_setup_load
   _phaseout_run_from_hookdir env

   phaseout_setup_single
   F_PHASEOUT_HOOK_FILTER=_phaseout_run_default_filter_env
   _phaseout_run_from_hookdir baselayout

   if [ "${PHASEOUT_FORCE_SINGLE:?}" = "y" ]; then
      phaseout_setup_single ## nop
   else
      phaseout_setup_parallel
   fi
   F_PHASEOUT_HOOK_FILTER=_phaseout_run_default_filter_non_parallel
   _phaseout_run_all_from_hookdir

   phaseout_setup_single
   F_PHASEOUT_HOOK_FILTER=_phaseout_run_default_filter_env_baselayout
   _phaseout_run_from_hookdir fini
}
