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


if have_kernel_module_loaded cpuid; then
   @@NOP@@

elif ! kernel_modules_have_kmod_dir; then
   veinfo "No kernel modules available - cannot detect whether cpuid is loaded."

elif modprobe cpuid; then
   @@NOP@@

else
   ewarn "Could not load the 'cpuid' kernel module (rc=${?})."
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
