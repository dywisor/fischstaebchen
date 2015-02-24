## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

phaseout_setup__common() {
   loadscript_zap_env

   X_PHASEOUT_LOADSCRIPT_STATUS=main

   F_PHASEOUT_HOOK_FILTER=
   PHASEOUT_HOOK_MODE=

   F_LOADSCRIPT_FILTER=__phaseout_filter
   F_LOADSCRIPT_SETENV_HOOK=__phaseout_setenv_hook
   F_LOADSCRIPT_SUCCESS_HOOK=__phaseout_success
   ##F_LOADSCRIPT_SUCCESS=
}

phaseout_setup_single() {
   phaseout_setup__common
   F_LOADSCRIPT_SETENV_PRE=__phaseout_single_setenv_pre
   LOADSCRIPT_DETACH=n
   PHASEOUT_HOOK_MODE=single
}

<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
phaseout_setup_parallel() {
   phaseout_setup__common
   F_LOADSCRIPT_SETENV_PRE=__phaseout_parallel_setenv_pre
   LOADSCRIPT_DETACH=y
   PHASEOUT_HOOK_MODE=parallel
}
<% else %>
phaseout_setup_parallel() {
   phaseout_setup_single "${@}"
}
<% endif %>


phaseout_setup_load() {
   phaseout_setup_single "${@}"
   PHASEOUT_HOOK_MODE=load
}
