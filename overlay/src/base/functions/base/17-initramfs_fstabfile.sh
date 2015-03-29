## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int get_static_fstab_file (
##    "newroot"|"initramfs"|"aux", name, **static_fstab_file!
## )
get_static_fstab_file() {
   static_fstab_file="${ICFG}/${1}-${2}.fstab"
   [ -f "${static_fstab_file}" ]
}

## int get_runtime_fstab_file (
##    "newroot"|"initramfs"|"aux", name, **runtime_fstab_file!
## )
get_runtime_fstab_file() {
   runtime_fstab_file="${ISHARE_CFG}/${1}-${2}.fstab"
   [ -f "${runtime_fstab_file}" ]
}

## int get_create_fstab_file (
##    "newroot"|"initramfs"|"aux", name,
##    **static_fstab_file!, **runtime_fstab_file!
## )
get_create_fstab_file() {
   if get_runtime_fstab_file "$@"; then
      get_static_fstab_file "$@" || @@NOP@@
      return 0
   elif get_static_fstab_file "$@"; then
      cat "${static_fstab_file}" >> "${runtime_fstab_file}"
   else
      touch "${runtime_fstab_file}"
   fi
}

addmnt() {
   [ -n "${1-}" ] && [ -n "${2-}" ] && [ -n "${3-}" ] || return

   >> "${runtime_fstab_file}" printf "%s %s %s %s %s %s\n" \
      "${1}" "${2#${NEWROOT}}" "${3}" "${4:-defaults}" "${5:-0}" "${6:-0}"
}

xaddmnt() { autodie addmnt "$@"; }
