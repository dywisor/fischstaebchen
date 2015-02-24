## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @stdout _root_read_home_from_passwd_file ( passwd_file, user_name )
_root_read_home_from_passwd_file() {
   ## if getent had a --root parameter..
   awk -F ':' -v u="${2}" \
'
BEGIN { ex=1; }
( ($1 == ""u) && ($6 != "") ) { print $6; ex=0; exit; }
END { exit ex; }' "${1}"
}


_root_get_home_from_passwd_file() {
   v1="$(_root_read_home_from_passwd_file "${@}")" || return 1
   case "${v1}" in
      /*)
         v0="${ROOT%/}${v1}"
         return 0
      ;;
   esac

   v0=
   v1=
   return 2
}

root_get_home_from_passwd() {
   <%%varcheck 1 %>
   <%%retvar v0 v1 %>
   <%%local passwd_file=${ROOT%/}/etc/passwd %>
   [ -s "${passwd_file}" ] || return 3
   _root_get_home_from_passwd_file "${passwd_file}" "${1}"
}

root_get_home() {
   <%%varcheck 1 %>
   <%%retvar v0 v1 %>
   <%%local passwd_file=${ROOT%/}/etc/passwd %>

   if [ -s "${passwd_file}" ]; then
      _root_get_home_from_passwd_file "${passwd_file}" "${1}" || return ${?}
      return 0

   elif [ "${1}" = "root" ]; then
      v0="${ROOT%/}/root"
      v1="/root"
      return 0

   else
      for v1 in "/home/${1}" "/var/users/${1}"; do
         v0="${ROOT%/}${v1}"

         if [ -d "${v0}" ] || [ -h "${v0}" ]; then
            return 0
         fi
      done

      v0=
      v1=
      return 3
   fi
}

root_get_home_not_devnull() {
   root_get_home "${@}" && [ "${v1}" != "/dev/null" ]
}

root_get_home_must_exist() {
   ##assert test ! -d /dev/null
   root_get_home "${@}" && [ -d "${v0}" ]
}
