## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if DYNAMIC_MESSAGE_FUNCTIONS=0 %>

# STUB
vrun_command() { "${@}"; }

<% else %>

if __quiet__ || ! __verbose__; then
vrun_command() { "${@}" @@QUIET@@; }
else
vrun_command() { "${@}"; }
fi


<% endif %>
