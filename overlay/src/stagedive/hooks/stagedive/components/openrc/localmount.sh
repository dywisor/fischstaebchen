## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

set --
while read -r fs mp fstype opts DONT_CARE; do
   case "${mp}" in
      "${ROOT}")
         set -- "${@}" /
      ;;
      "${ROOT%/}/"?*)
         set -- "${@}" "${mp#${ROOT%/}}"
      ;;
   esac
done < /proc/self/mounts

{
   if [ $# -gt 0 ]; then
      print_vassign no_umounts "${1:?}"
      shift

      while [ $# -gt 0 ]; do
         print_vassign no_umounts "\${no_umounts}:${1:?}"
         shift
      done
   fi
} > "${ROOT_ETC_CONFD}/localmount" || die "Failed to write localmount.sh"
