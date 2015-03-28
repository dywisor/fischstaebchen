## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

kernel_modules__get_kver() {
   kver=
   if [ -z "${__kernel_modules_kver-}" ]; then
      __kernel_modules_kver="$(uname -r @@QUIET@@)"
      [ -n "${__kernel_modules_kver}" ] || return 1
   fi

   kver="${__kernel_modules_kver}"
   return 0
}

# shellcheck disable=SC2120
kernel_modules__get_kver_and_mod_dir() {
   mod_dir=
   kernel_modules__get_kver "${@}" || return @@EX_NOT_SUPPORTED@@
   mod_dir="/lib/modules/${mod_dir}"
   [ -d "${mod_dir}" ] || return @@EX_NOT_SUPPORTED@@
}

## int have_kernel_module_loaded ( *module_name, **v0! )
have_kernel_module_loaded() {
   <%%retvar v0 %>
   <%%locals kver mod_dir %>

   # shellcheck disable=SC2119
   kernel_modules__get_kver_and_mod_dir || return
   [ -f "${mod_dir}/modules.builtin" ] || return @@EX_NOT_SUPPORTED@@
   [ -e /proc/modules ] || return @@EX_NOT_SUPPORTED@@

   while [ $# -gt 0 ]; do
      if [ -z "${1}" ]; then
         @@NOP@@

      elif grep -q -- "/${1}[.]ko\$" "${mod_dir}/modules.builtin"; then
         @@NOP@@

      elif grep -Eq -- "^${1}\s" "/proc/modules"; then
         @@NOP@@

      else
         <%%vappend v0 ${1} %>
      fi

      shift
   done

   [ -z "${v0}" ]
}
