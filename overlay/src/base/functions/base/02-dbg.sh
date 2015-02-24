## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## do not use the functions by this file directly,
## rely on DBGTRACE, DBGTRACE_FUNC, DBGTRACE_CMD instead.
##
<%if DBGTRACE %>


__dbgtrace_message__printf_stderr() {
   printf "%s %s\n" "${2}" "${1}" 1>&2
}


## int __dbgtrace_message_emitter ( msg_func, extra_header, *message )
##
__dbgtrace_message_emitter() {
   [ $# -gt 1 ] && [ -n "${1-}" ] || return @@EX_USAGE@@
   [ $# -gt 2 ] || return 0

   <%%locals msg_func< msg_header msg_extra_header<=${1} %>

   if [ -n "${msg_extra_header}" ]; then
      msg_header="[DBGTRACE:${msg_extra_header}]"
   else
      msg_header="[DBGTRACE]"
   fi
   msg_header="${msg_header} [${__dbgtrace_depth:-U}]"

   [ "${HAVE_MESSAGE_FUNCTIONS:-X}" = "y" ] || \
      msg_func=__dbgtrace_message__printf_stderr

   if [ -n "${DBGTRACE_LOGFILE-}" ] && \
      {
         [ -z "${DBGTRACE_LOGFILE%/*}" ] || \
         mkdir -p -- "${DBGTRACE_LOGFILE%/*}"
      }
   then
      while [ $# -gt 0 ]; do
         if [ -n "${1}" ]; then
            printf "%s\n" "dbgtrace: ${msg_extra_header:-undef}: ${1}" \
               >> "${DBGTRACE_LOGFILE:?}" || @@NOP@@

            "${msg_func:?}" "${1}" "${msg_header}" || @@NOP@@
         fi

         @@SHIFT_OR_RET@@
      done

   else
      while [ $# -gt 0 ]; do
         [ -z "${1}" ] || "${msg_func:?}" "${1}" "${msg_header}" || @@NOP@@

         @@SHIFT_OR_RET@@
      done
   fi
}

__dbgtrace_do_print_status() {
   __dbgtrace_message_emitter "${1:-einfo}" "${2:-status}" \
         "rc=${__dbgtrace_rc}" \
         "v0=${v0-}" \
         "functrace=${__dbgtrace_functrace-}"
}

__dbgtrace_do_print_run() {
   __dbgtrace_message_emitter einfo run \
      "cmd=${*}" \
      "previous rc=${__dbgtrace_rc}" \
      "previous v0=${__dbgtrace_v0}" \
      "functrace=${__dbgtrace_functrace-}"
}

__dbgtrace_do_print_run_done() {
   if [ ${__dbgtrace_rc} -eq 0 ]; then
      __dbgtrace_message_emitter einfo run_success "cmd=${*}"
      __dbgtrace_do_print_status einfo run_success

   else
      __dbgtrace_message_emitter ewarn run_failed "cmd=${*}"
      __dbgtrace_do_print_status ewarn run_failed
   fi
}

<%if DBGTRACE_CMD= %>
__dbgtrace() {
   local __dbgtrace_rc=${?}
   local __dbgtrace_depth=$(( ${__dbgtrace_depth:-0} + 1 ))
   local __dbgtrace_v0="${v0-}"

   if [ $# -eq 0 ]; then
      __dbgtrace_do_print_status
   else
      __dbgtrace_do_print_run "${@}"
      "${@}" && __dbgtrace_rc=${?} || __dbgtrace_rc=${?}
      __dbgtrace_do_print_run_done
   fi

   return ${__dbgtrace_rc}
}
<%endif %>

<%if DBGTRACE_FUNC= %>
__dbgtrace_function() {
   local __dbgtrace_rc=${?}
   local __dbgtrace_depth=$(( ${__dbgtrace_depth:-0} + 1 ))
   local __dbgtrace_v0="${v0-}"
   local __dbgtrace_func="${1-}"
   local __dbgtrace_old_functrace="${__dbgtrace_functrace-}"

   if [ -z "${1-}" ]; then
      __dbgtrace_message_emitter eerror function "function name is empty"
      __dbgtrace_do_print_status eerror function
      exit 250
   fi

   __dbgtrace_do_print_run "${@}"

   local __dbgtrace_functrace
   __dbgtrace_functrace="${__dbgtrace_old_functrace} ${__dbgtrace_func}"

   "${@}" && __dbgtrace_rc=${?} || __dbgtrace_rc=${?}

   __dbgtrace_functrace="${__dbgtrace_old_functrace}"

   __dbgtrace_do_print_run_done "${@}"

   if [ ${__dbgtrace_rc} -ne 0 ]; then
      if \
         <%%function_defined ${__dbgtrace_func} %>
      then
         @@NOP@@
      else
         __dbgtrace_message_emitter eerror function "function ${1-} is not defined"
         exit 251
      fi
   fi

   return ${__dbgtrace_rc}
}
<%endif %>

<%endif %>
