## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int kernel_modules_get_kver ( **kver!, **__kernel_modules_kver!g )
##
##  Loads the kernel release string as returned by "uname -r"
##  to the (should-be local) %kver variable.
##  Uses the global %__kernel_modules_kver variable for caching the result.
##
kernel_modules_get_kver() {
   kver=
   if [ -z "${__kernel_modules_kver-}" ]; then
      __kernel_modules_kver="$(uname -r @@QUIET@@)"
      [ -n "${__kernel_modules_kver}" ] || return 1
   fi

   kver="${__kernel_modules_kver}"
   return 0
}

## int kernel_modules_get_kmod_dir (
##    kmod_root:=INIT_KERNEL_MODULES_DIR,
##    **kmod_dir!, **__kernel_modules_kver!g
## )
##
kernel_modules_get_kmod_dir() {
   <%%retvar kmod_dir %>
   <%%locals kver kmod_root=${2-${INIT_KERNEL_MODULES_DIR?}} %>

   [ -n "${kmod_root}" ] || return @@EX_NOT_SUPPORTED@@
   kernel_modules_get_kver || return ${?}

   kmod_dir="${kmod_root%/}/${kver}"
   [ -d "${kmod_dir}" ]
}

## int kernel_modules_have_kmod_dir (
##    kmod_root:=INIT_KERNEL_MODULES_DIR, **__kernel_modules_kver!g
## )
##
kernel_modules_have_kmod_dir() {
   <%%locals kmod_dir %>
   kernel_modules_get_kmod_dir "${@}"
}

## int kernel_modules_check_module_loaded ( *module_name, **v0! )
##
kernel_modules_check_module_loaded() {
   <%%retvar v0 %>
   <%%locals kmod_dir kmod_builtin kmod_loaded %>

   kmod_builtin=
   if kernel_modules_get_kmod_dir; then
      kmod_builtin="${kmod_dir}/modules.builtin"
      [ -f "${kmod_builtin}" ] || kmod_builtin=
   fi

   kmod_loaded="/proc/modules"
   [ -e "${kmod_loaded}" ] || kmod_loaded=

   while [ $# -gt 0 ]; do
      if [ -z "${1}" ]; then
         @@NOP@@

      elif \
         [ -n "${kmod_builtin}" ] && \
         grep -q -- "/${1}[.]ko\$" "${kmod_builtin}"
      then
         @@NOP@@

      elif \
         [ -n "${kmod_loaded}" ] && \
         grep -Eq -- "^${1}\s" "${kmod_loaded}"
      then
         @@NOP@@

      else
         <%%vappend v0 ${1} %>
      fi

      shift
   done

   [ -z "${v0}" ]
}

## int have_kernel_module_loaded ( *module_name, **v0! )
have_kernel_module_loaded() {
   kernel_modules_check_module_loaded "${@}"
}
