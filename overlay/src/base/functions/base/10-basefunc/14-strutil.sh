## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

io_first_whitespace_field() { sed -r -e 's,\s+.*$,,'; }

yesno() {
   case "${1-}" in
      [yY]|\
      1|\
      [yY][eE][sS]|\
      [tT][rR][uU][eE]|\
      [oO][nN]|\
      [eE][nN][aA][bB][lL][eE][dD])
         return 0
      ;;
   esac

   return 1
}

## @setf void get_regex_or_expr ( *wordlist, **expr! )
get_regex_or_expr() {
   expr=
   # shellcheck disable=SC2048
   { set -f; set -- ${*}; set +f; }
   [ ${#} -gt 0 ] || return 1

   expr="${1}"; shift
   <%%argc_loop ::: expr="${expr}|${1}" %>
   expr="(${expr})"
   return 0
}

## @setf void get_regex_or_expr_exact ( *wordlist, **expr! )
get_regex_or_expr_exact() {
   get_regex_or_expr "${@}"
   [ -z "${expr}" ] || expr="^${expr}\$"
}
