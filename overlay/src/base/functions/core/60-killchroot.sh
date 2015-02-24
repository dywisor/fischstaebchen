## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _find_chroot_processes ( chroot_dir, **chroot_dir!, **v0! )
_find_chroot_processes() {
   <%%retvars chroot_dir v0 %>
   <%%locals proc_root_link proc_root pid %>

   : ${1:?}
   chroot_dir="$(readlink -f "${1}")"
   : ${chroot_dir:?}
   [ "${chroot_dir}" != "/" ] || return @@EX_USAGE@@

   # argv := pid list
   set --

   for proc_root_link in /proc/*/root; do
      proc_root="$(readlink -f "${proc_root_link}")"

      if [ "${proc_root:-X}" = "${chroot_dir}" ]; then
         pid="${proc_root_link%/root}"; pid="${pid##*/}"

         case "${pid}" in
            1|$$)
               ewarn "_find_chroot_processes(${chroot_dir}): ignoring pid ${pid}"
            ;;

            *)
               set -- "${@}" "${pid}"
            ;;
         esac
      fi
   done

   v0="${*}"
}

## int find_chroot_processes ( chroot_dir, **v0! )
find_chroot_processes() {
   <%%retvar v0 %>
   <%%locals chroot_dir %>
   _find_chroot_processes "${@}"
}


## kill_chroot_processes ( ["-q"|"--quiet"], chroot_dir )
kill_chroot_processes() {
   <%%locals quiet chroot_dir pid v0 %>

   quiet=
   case "${1-}" in
      '-q'|'--quiet') quiet=y; shift || die ;;
   esac

   _find_chroot_processes "${@}" || return ${?}

   set -- ${v0}

   if [ ${#} -eq 0 ]; then
      [ -n "${quiet}" ] || \
         veinfo "kill_chroot_processes(${chroot_dir}): no active processes found."

      return 0
   fi

   [ -n "${quiet}" ] || ewarn "Found leftover processes in ${chroot_dir}: ${*}"

   [ -n "${quiet}" ] || einfo "Sending SIGTERM: ${*}"
   kill -15 "${@}"

   <%weakdef CHROOT_KILL_DELAY 1 %>
   <% if CHROOT_KILL_DELAY %>
   sleep @@CHROOT_KILL_DELAY@@ @@QUIET@@ || @@NOP@@
   <% endif %>

   ## Note: kill -9 may "fail" if %pid terminates before receiving sigkill;
   ##        the [ -e /proc/%pid ] check is racy but sufficient here
   if [ -n "${quiet}" ]; then
      kill -9 "${@}" @@QUIET@@ || @@NOP@@

   else
      v0=
      <%%argc_loop ::: [ ! -e "/proc/${1}" ] || v0="${v0} ${1}" %>
      set -- ${v0}

      if [ ${#} -gt 0 ]; then
         ewarn "Sending SIGKILL: ${*}"
         kill -9 "${@}" || @@NOP@@
      else
         einfo "All processes terminated cleanly."
      fi
   fi
}
