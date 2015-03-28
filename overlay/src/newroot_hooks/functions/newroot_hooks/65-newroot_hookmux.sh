## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<% if ALLOW_NEWROOT_HOOKS %>

## int newroot_do_run_hooks ( *files )
##
newroot_do_run_hooks() {
   <%%while_argc +++ newroot_do_run_hook "${1}" %>
}

## int newroot_rw_do_run_hooks ( *files )
##
newroot_rw_do_run_hooks() {
   [ ${#} -gt 0 ] || return 0
   # shellcheck disable=SC2031
   autodie get_newroot_rw && _newroot_do_run_hooks "${@}"
}

## int newroot_do_run_hooks_if_exist ( *files )
##
newroot_do_run_hooks_if_exist() {
   while [ $# -gt 0 ]; do
      if test_fs_is_file "${1}"; then
         newroot_do_run_hook "${1}" || return ${?}
      fi

      shift
   done
}

## int newroot_rw_do_run_hooks_if_exist ( *files )
##
newroot_rw_do_run_hooks_if_exist() {
   # racy
   # shellcheck disable=SC2031
   [ ${#} -gt 0 ] && ( get_any_file "${@}"; ) || return 0

   # shellcheck disable=SC2031
   get_newroot_rw && \
   newroot_do_run_hooks_if_exist "${@}"
}

## newroot_run_hooks ( phase )
newroot_run_hooks() {
   <%%locals hook_phase=${1:?} hook_dir%>
   hook_dir="${NEWROOT_HOOKDIR}/${hook_phase}"

   if test_fs_is_dir "${hook_dir}"; then
      einfo "Running hooks from ${hook_dir}/"
      autodie newroot_rw_do_run_hooks_if_exist "${hook_dir}/"* || return

   elif test_fs_exists "${hook_dir}"; then
      ewarn "newroot hook dir is not a dir: ${hook_dir}"

   else
      veinfo "Not running ${hook_phase} hooks: hook dir does not exist."
      veinfo "  (${hook_dir})"
   fi

   return 0
}

<% endif %>
