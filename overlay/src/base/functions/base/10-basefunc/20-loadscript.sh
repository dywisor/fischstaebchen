## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

loadscript_zap_env() {
   ##__LOADSCRIPT_SUBSHELL=
   F_LOADSCRIPT_FILTER=
   F_LOADSCRIPT_SETENV_PRE=
   F_LOADSCRIPT_SETENV_HOOK=
   ##F_LOADSCRIPT_DO_LOAD=
   F_LOADSCRIPT_SUCCESS_HOOK=
   ##F_LOADSCRIPT_SUCCESS=
   LOADSCRIPT_DETACH=n
   ##X_PHASEOUT_LOADSCRIPT_STATUS=
}
loadscript_zap_env

__loadscript_do_load() {
   local fail
   local X_PHASEOUT_LOADSCRIPT_STATUS="${X_PHASEOUT_LOADSCRIPT_STATUS:-X}"

   ${F_LOADSCRIPT_SETENV:-@@NOP@@} || return ${?}
   ## note that "test -f" filters out devices, pipes,
   ## but also "/proc/self/fd/0" (stdin)
   [ -f "${__FILE__:?}" ] || return 1

   if [ "${X_PHASEOUT_LOADSCRIPT_STATUS}" = "main" ]; then
      X_PHASEOUT_LOADSCRIPT_STATUS=sub
      . "${__FILE__:?}" && fail=0 || fail=${?}
      X_PHASEOUT_LOADSCRIPT_STATUS=main
   else
      . "${__FILE__:?}" && fail=0 || fail=${?}
   fi

   [ ${fail} -eq 0 ] || return ${fail}

   ${F_LOADSCRIPT_SUCCESS_HOOK:-@@NOP@@}
}

__loadscript() {
   : ${__LOADSCRIPT_SUBSHELL:?}
   <%%locals __FILE__ __FNAME__ __NAME__ __DIR__ %>

   __loadscript_simple_set_file_vars "${1:?}" || return ${?}

   ## we do not "test -f %__FILE__" here,
   ## this allows lazy script generation.

   ${F_LOADSCRIPT_FILTER:-@@NOP@@} "${__NAME__}" || return 0

   ${F_LOADSCRIPT_SETENV_PRE:-@@NOP@@} || return ${?}

   if [ "${LOADSCRIPT_DETACH:-n}" = "y" ]; then
      ## no stdin
      __loadscript_do_load &

   elif [ "${__LOADSCRIPT_SUBSHELL:?}" = "y" ]; then
      ( __loadscript_do_load; )

   else
      __loadscript_do_load
   fi
}

loadscript() {
   <%%locals __LOADSCRIPT_SUBSHELL=n %>
   __loadscript "${@}"
}

loadscript_or_die() {
   loadscript "${@}" || die "Failed to load ${1:-???}."
}

loadscript_if_exists() {
   [ ! -f "${1:?}" ] || loadscript "${@}"
}

loadscript_if_exists_or_die() {
   loadscript_if_exists "${@}" || die "Failed to load ${1:-???}."
}


runscript() {
   <%%locals __LOADSCRIPT_SUBSHELL=y %>
   __loadscript "${@}"
}

runscript_or_die() {
   runscript "${@}" || die "Failed to run ${1:-???}."
}

runscript_if_exists() {
   [ ! -f "${1:?}" ] || runscript "${@}"
}

runscript_if_exists_or_die() {
   [ ! -f "${1:?}" ] || runscript_or_die "${@}"
}


loadscripts_if_exist_or_die() {
   <%%locals __LOADSCRIPT_SUBSHELL=n %>
   <%% argc_loop ::: !\
      | [ ! -f "${1}" ] || __loadscript "${1}" || die "Failed to load ${1}". %>
}

runscripts_if_exist_or_die() {
   <%%locals __LOADSCRIPT_SUBSHELL=y %>
   <%% argc_loop ::: !\
      | [ ! -f "${1}" ] || __loadscript "${1}" || die "Failed to run ${1}". %>
}
