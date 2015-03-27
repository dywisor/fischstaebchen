## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

phaseout_setup__zap() {
   F_PHASEOUT_HOOK_FILTER=
   PHASEOUT_HOOK_MODE=
}

phaseout_setup_load() {
   phaseout_setup__zap

   PHASEOUT_HOOK_MODE=load
}

phaseout_setup_single() {
   phaseout_setup__zap

   PHASEOUT_HOOK_MODE=single
}

<% if PHASEOUT_RUN_HOOKS_IN_PARALLEL %>
phaseout_setup_parallel() {
   phaseout_setup__zap

   PHASEOUT_HOOK_MODE=parallel
}
<% else %>
phaseout_setup_parallel() {
   phaseout_setup_single "${@}"
}
<% endif %>
