## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

__phaseout_do_run_hook() {
   local retcode

   retcode=0
   case "${PHASEOUT_HOOK_MODE:-}" in
      '')
         die "phaseout_setup() must be called before run()."
      ;;

      single)
         if runscript "${@}"; then
            veinfo "run ${1}: success"
         else
            retcode=${?}
            eerror "Failed to run ${1}."
         fi
      ;;

<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
      parallel)
         if runscript "${@}"; then
            veinfo "start ${1}: success"
         else
            retcode=${?}
            eerror "Failed to start ${1}."
         fi
      ;;
<% endif %>

      load)
         if loadscript "${@}"; then
            veinfo "load ${1}: success"
         else
            retcode=${?}
            eerror "Failed to load ${1}"
         fi
      ;;

      *)
         die "unknown hook mode: ${PHASEOUT_HOOK_MODE}"
      ;;
   esac
   return ${retcode}
}

_phaseout_waitfor_hooks() {
<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
   case "${PHASEOUT_HOOK_MODE:?}" in
      parallel)
         einfo "Waiting for hooks to finish"
         wait || die "wait() returned ${?}"
      ;;
   esac
<% endif %>
   return 0
}

_phaseout_get_failed() {
   <%%retvar failed%>
   <%%locals f%>

   set --
   <%%foreach f "${PHASEOUT_HOOK_SYNCDIR_PENDING}/"* ::: !\
      | [ ! -f "${f}" ] || set -- "${@}" "${f}" %>

   [ ${#} -gt 0 ] && failed="${*}"
}

_phaseout_report_failed() {
   set -f; set -- ${*}; set +f;
   [ ${#} -gt 0 ] || return @@EX_USAGE@@

   eerror "The following hooks did not succeed:"

   while [ ${#} -gt 0 ]; do
      if [ -s "${1}" ]; then
         eerror "${1##*/}: $(cat "${1}")" "-"
      else
         eerror "${1##*/}" "-"
      fi

      shift
   done
}

_phaseout_get_and_report_failed() {
   _phaseout_get_failed || return 0
   autodie _phaseout_report_failed "${failed}"
   return 1
}

_phaseout_run() {
   <%%locals failcode keep_going %>

   keep_going=
   case "${1-}" in
      '-k'|'--keep-going') keep_going=y; @@SHIFT_OR_RET@@ ;;
   esac

   failcode=0
   while [ ${#} -gt 0 ]; do
      if [ ! -f "${1}" ]; then
         eerror "hook file does not exist: ${1}"
         failcode=127
         [ -n "${keep_going}" ] || return ${failcode}

      elif __phaseout_do_run_hook "${1}"; then
         @@NOP@@

      else
         failcode=${?}
         [ -n "${keep_going}" ] || return ${failcode}
      fi

      shift
   done

   return ${failcode}
}

_phaseout_run_if_exist() {
   <%%locals failcode keep_going %>

   keep_going=
   case "${1-}" in
      '-k'|'--keep-going') keep_going=y; @@SHIFT_OR_RET@@ ;;
   esac

   failcode=0
   while [ ${#} -gt 0 ]; do
      if [ ! -f "${1}" ]; then
         @@NOP@@

      elif __phaseout_do_run_hook "${1}"; then
         @@NOP@@

      else
         failcode=${?}
         [ -n "${keep_going}" ] || return ${failcode}
      fi

      shift
   done

   return ${failcode}
}
