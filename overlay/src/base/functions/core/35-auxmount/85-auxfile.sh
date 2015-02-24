## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int auxmount_get_filepath ( uri, **v0! )
auxmount_get_filepath() {
   <%%retvar v0 %>
   <%%locals type srcpath -p uri_ _ basepath filepath %>

   parse_file_uri "$@" && \
   auxmount_do_get_filepath "${type}" "${uri_basepath}" "${uri_filepath}"
}

## int auxmount_do_get_filepath ( type, mount_uri, file_relpath, **v0! )
##
auxmount_do_get_filepath() {
   <%%retvar v0 %>
   <%%locals v1 %>

   [ -n "${1-}" ] && [ -n "${2-}" ] && [ -n "${3-}" ] || return @@EX_USAGE@@

   auxmount none "${1}" "${2}" || return ${?}

   v0="${v0}/${3#/}"
}


get_auxfile_path() {
   if [ -n "${2+SET}" ]; then
      vget_auxmount "${1}" && v0="${v0:?}/${2#/}"
   else
      vget_auxmount "${1%%/*}" && v0="${v0:?}/${1#*/}"
   fi
}

get_auxfile() {
   get_auxfile_path "$@" && [ -f "${v0}" ]
}
