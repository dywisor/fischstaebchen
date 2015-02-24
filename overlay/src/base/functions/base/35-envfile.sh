## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int __envfile_add ( env_file, key, val )
__envfile_add() {
   print_vassign "${2:?}" "${3?}" >> "${1:?}"
}

## @autodie envfile_add ( env_file, key, val )
envfile_add() {
   autodie __envfile_add "${@}"
}

## @autodie envfile_add_var ( env_file, *varname )
##
if __debug__; then
envfile_add_var() {
   ## local %val not necessary
   <%%locals env_file< val%>

   while [ $# -gt 0 ]; do
      if [ -n "${1}" ]; then
         (
            eval "val=\"\${${1}?}\"" && \
            envfile_add "${env_file}" "${1}" "${val}"
         ) || die "envfile_add_var() failed to eval/add '${1}'"
      fi
      shift
   done
}
else
envfile_add_var() {
   <%%locals env_file< val%>

   while [ $# -gt 0 ]; do
      if [ -n "${1}" ]; then
         eval "val=\"\${${1}?}\"" && \
         envfile_add "${env_file}" "${1}" "${val}"
      fi
      shift
   done
}
fi
