## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @setf int __read_text_file_call_func (
##    function, *args, **infile, **line, **lino
## )
##
__read_text_file_call_func() {
   ##: ${1:?}
   set -f; set -- "${@}" ${line}; set +f
   "${@}"
}

__read_text_file_filter_line() {
   case "${line}" in
      ''|'#'*) return 1 ;;
   esac
   return 0
}


## @setf int read_text_file_do (
##    infile, function, *args, (**infile!), (**line!), (**lino!)
## )
##
read_text_file_do() {
   [ -n "${1-}" ] && [ -n "${2-}" ] || return @@EX_USAGE@@
   <%%locals infile< line lino=0 %>
   while read -r line; do
      <%% inc lino %>

      if __read_text_file_filter_line; then
         __read_text_file_call_func "${@}" || return ${?}
      fi
   done < "${infile}"
}

## int read_text_file_v0 ( infile, data_sep=" ", **v0! )
##
read_text_file_v0() {
   <%%retvar v0 %>
   [ -n "${1-}" ] || return @@EX_USAGE@@
   <%%locals line %>

   while read -r line; do
      if __read_text_file_filter_line; then
         v0="${v0:+${v0}${2- }}${line}"
      fi
   done < "${1}"
}
