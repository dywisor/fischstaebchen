## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

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
   # shellcheck disable=SC2048
   { set -f; set -- ${*}; set +f; }
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
   <%%locals failcode keep_going=y ignore_missing= %>
   while [ ${#} -gt 0 ]; do
      case "${1-}" in
         '-k'|'--keep-going')      keep_going=y;     @@SHIFT_OR_RET@@ ;;
         '-e'|'--ignore-missing')  ignore_missing=y; @@SHIFT_OR_RET@@ ;;
         '--') @@SHIFT_OR_RET@@; break ;;
         *) break ;;
      esac
   done

   if [ -z "${ignore_missing}" ]; then
      [ -n "${keep_going}" ] && ignore_missing=fail || ignore_missing=error
   fi

   if __debug__; then
      veinfo "phaseout_run(): keep_going=${keep_going}"
      veinfo "phaseout_run(): ignore_missing=${ignore_missing}"
   fi

   failcode=0
   while [ ${#} -gt 0 ]; do
      if __phaseout_run_hook_dispatch "${1}" "${ignore_missing}"; then
         @@NOP@@

      else
         failcode=${?}
         [ -n "${keep_going}" ] || return ${failcode}
      fi

      shift
   done

   return ${failcode}
}
