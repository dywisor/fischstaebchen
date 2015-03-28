## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if ALLOW_NEWROOT_HOOKS %>

newroot_run_hook__subshell() {
   # shellcheck disable=SC2030
   (
      newroot_hook__setup_env_do_export initramfs
      set -- "${NEWROOT}"

      loadscript_simple "${hook_file}"
   )
}

newroot_run_hook__exec() {
   (
      newroot_hook__setup_env_do_export initramfs

      exec "${hook_file}" "${NEWROOT}"
   )
}

newroot_run_hook__loadscript() {
   local D
   local IN_NEWROOT

   newroot_hook__setup_env_initramfs

   loadscript_simple "${hook_file}"
}

newroot_run_hook__chroot_exec() {
   <%%locals newroot_hook_file_rel copied_hook_file%>

   copied_hook_file=
   case "${hook_file}" in
      "${NEWROOT}/"*)
         newroot_hook_file_rel="${hook_file#${NEWROOT}}"
      ;;

      *)
         newroot_hook_file_rel="/tmp/initramfs_hook.exec"
         copied_hook_file="${NEWROOT}${newroot_hook_file_rel}"

         if ! cp -- "${hook_file}" "${copied_hook_file}"; then
            die "Cannot chroot-exec '${hook_file}': failed to import file."
            return

         elif ! {
            chmod 0500 "${copied_hook_file}" && \
            chown 0:0  "${copied_hook_file}"
         }; then

            autodie rm -- "${copied_hook_file}"
            die "Cannot chroot-exec '${hook_file}': failed to chmod/chown file."
            return
         fi
      ;;
   esac

   (
      newroot_hook__setup_env_do_export newroot

      exec chroot "${NEWROOT}" "${newroot_hook_file_rel}" /
   ) || hook_rc=${?}

   if [ -n "${copied_hook_file}" ]; then
      if ! rm -- "${copied_hook_file}"; then
         if test_fs_exists "${copied_hook_file}"; then
            die "failed to remove ${copied_hook_file}"
            return
         fi
      fi
   fi

   return ${hook_rc}
}

newroot_do_run_hook() {
   <%%locals -p hook_ file=${1:?} name run_type rc %>
   hook_name="${hook_file##*/}"

   case "${hook_name}" in
      *.chroot*)
         hook_run_type=chroot_exec
      ;;
      *.subshell*)
         hook_run_type=subshell
      ;;
      *.sh)
         hook_run_type=loadscript
      ;;
      *)
         hook_run_type=exec
      ;;
   esac

   einfo "Running ${hook_run_type} hook '${hook_name}'"
   # __%hook_run_type() may set %hook_rc
   hook_rc=0
   "newroot_run_hook__${hook_run_type}" || hook_rc=${?}

   if [ ${hook_rc} -ne 0 ]; then
      eerror \
"Failed to run ${hook_run_type:-???} hook '${hook_name:-???}' (rc=${hook_rc})"
   else
      veinfo "Hook ${hook_name:-???} succeeded."
   fi

   return ${hook_rc}
}

<% endif %>
