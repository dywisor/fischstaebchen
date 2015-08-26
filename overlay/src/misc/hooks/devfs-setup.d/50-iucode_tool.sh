## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% weakdef IUCODE_TOOL /usr/sbin/iucode_tool %>
<% if HAVE_IUCODE_TOOL %>

if ! ishare_has_flag want-load-cpu-iucode; then
   veinfo "Not loading CPU microcode - disabled."
   return 0
fi

if ! kernel_modules_try_load_module cpuid; then
   return 0
fi

einfo "Loading microcode data"
if @@IUCODE_TOOL@@ -S; then
   @@NOP@@
<% if SELF_DESTRUCTIVE %>
   rm -f -- /usr/share/misc/intel-microcode.dat
<% endif %>
else
   eerror "Failed to load microcode data (rc=${?})"
fi

@@NOP@@
<% endif %>
