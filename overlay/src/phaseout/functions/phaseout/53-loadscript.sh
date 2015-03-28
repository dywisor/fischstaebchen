## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

__phaseout_run_hook__success() {
   # do not use __FAILFILE__ here.
   autodie rm -- "${PHASEOUT_HOOK_SYNCDIR_PENDING}/${__NAME__}"
   autodie touch -- "${PHASEOUT_HOOK_SYNCDIR_SUCCESS}/${__NAME__}"
}

__phaseout_run_hook__loadscript() {
   . "${__FILE__}" && __phaseout_run_hook__success
}

__phaseout_run_hook__runscript() {
   ( export TMPDIR; export T; __phaseout_run_hook__loadscript; )
}

## int __phaseout_run_hook_dispatch ( file_arg, ignore_missing:="n" ), raises die()
##
__phaseout_run_hook_dispatch() {
   case "${PHASEOUT_HOOK_MODE-}" in
      load|single|parallel)
         @@NOP@@
      ;;
      *)
         die "phaseout_setup() must be called before run()."
      ;;
   esac

   <%%locals __FILE__ __FNAME__ __NAME__ __DIR__  %>
   __loadscript_simple_set_file_vars "${1:?}" || return ${?}

   <%%locals !\
      | __FAILFILE__=${PHASEOUT_HOOK_SYNCDIR_PENDING}/${__NAME__} !\
      | __GRP__=${__NAME__%%-*} !\
      | __GRPSYNCDIR__=${PHASEOUT_HOOK_SYNCDIR_GRP}/${__GRP__} !\
      | __phaseout_tmpdir=${__GRPSYNCDIR__}/${__NAME__} !\
   %>

   <%%locals T TMPDIR is_new_grp retcode %>

   if [ -n "${F_PHASEOUT_HOOK_FILTER-}" ]; then
      if ! ${F_PHASEOUT_HOOK_FILTER} "${__NAME__}"; then
         einfo "hook filtered out: ${__NAME__}"
         return 0
      fi
   fi

   # check if __FILE__ exists
   if [ ! -f "${__FILE__}" ]; then
      case "${2-}" in
         ''|'n'|'fail')
            eerror "Cannot load/run hook '${__FILE__}': file not found."
            # set hook status to "failed"
            printf '%s\n' 'file-missing' > "${__FAILFILE__}" || die "#mkfailfile"
            return 127
         ;;
         'error')
            eerror "Cannot load/run hook '${__FILE__}': file not found."
            return 127
         ;;
         'warn')
            warn "Cannot load/run hook '${__FILE__}': file not found."
            return 0
         ;;
         'y')
            veinfo "Cannot load/run hook '${__FILE__}': file not found."
            return 0
         ;;
      esac
   fi

   # initially, set hook status to "failed"
   autodie touch -- "${__FAILFILE__}"

   # when running in parallel,
   #  do not output a message for each hook group (too noisy!),
   #  output per-hook-group messages instead
   is_new_grp=
   case "${PHASEOUT_HOOK_MODE-}" in
<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
      parallel)
         ! mkdir -- "${__GRPSYNCDIR__}" @@QUIET@@ || is_new_grp=y
      ;;
   esac
<% endif %>

   # create tmpdir
   autodie mkdir -p -- "${__phaseout_tmpdir}"
   TMPDIR="${__phaseout_tmpdir}"
   T="${__phaseout_tmpdir}"

   # run hook
   retcode=0
   case "${PHASEOUT_HOOK_MODE-}" in
      load)
         einfo "Loading ${PHASEOUT_HOOK_NAMESPACE:-???} file: ${__NAME__}"

         if __phaseout_run_hook__loadscript; then
            veinfo "Load ${__FILE__:-???}: success"
         else
            retcode=${?}
            eerror "Failed to load ${__FILE__:-???}."
         fi
      ;;

      single)
         einfo "Running ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${__NAME__}"

         if __phaseout_run_hook__runscript; then
            veinfo "Run ${__FILE__}: success"
         else
            retcode=${?}
            eerror "Failed to run ${__FILE__}."
         fi
      ;;

<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
      parallel)
         if [ "${__GRP__}" != "${__NAME__}" ]; then
            if [ "${is_new_grp:-X}" = "y" ]; then
               einfo "Starting ${PHASEOUT_HOOK_NAMESPACE:-???} hooks: ${__GRP__}-*"
            fi

            veinfo "Starting ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${__NAME__}"
         else
            einfo "Starting ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${__NAME__}"
         fi

         if { __phaseout_run_hook__runscript & }; then
            veinfo "Start ${__FILE__}: success"
         else
            retcode=${?}
            eerror "Failed to start ${__FILE__}."
         fi
      ;;
<% endif %>

      *) die "bad PHASEOUT_HOOK_MODE." ;;
   esac

   return ${retcode}
}
