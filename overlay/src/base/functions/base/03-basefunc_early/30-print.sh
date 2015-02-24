## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

print_line()        { printf '%s\n'    "${*}"; }
print_line_indent() { printf '   %s\n' "${*}"; }

print_vassign() {
   printf '%s=%s%s%s\n' "${1:?}" "\"" "${2?}" "\""
}

print_vassigns() {
   <%%locals val %>

   while [ $# -gt 0 ]; do
      case "${1}" in
         '')
            printf '\n'
         ;;
         *=*)
            print_vassign @@KEYOF1@@ @@VALOF1@@
         ;;
         *)
            <%% loadvar val ${1}? %>
            print_vassign "${1}" "${val}"
         ;;
      esac

      shift
   done
}
