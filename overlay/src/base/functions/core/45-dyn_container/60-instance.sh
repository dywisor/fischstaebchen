## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

dyncontainer_zap_vars() {
   DYNCONTAINER_PATH=
   DYNCONTAINER_NAME=
   DC=
}

dyncontainer_init() {
   dyncontainer_zap_vars
   DYNCONTAINER_PATH="${1}"
   DYNCONTAINER_NAME="${2:-dyco}"

   autodie _dyncontainer_init \
      "${DYNCONTAINER_PATH}" "${DYNCONTAINER_NAME}" "${3-}"

   DC="${DYNCONTAINER_PATH}"
}

dyncontainer_get_size() {
   _dyncontainer_get_size "${DYNCONTAINER_PATH:?}" "$@"
}

dyncontainer_downsize() {
   _dyncontainer_downsize "${DYNCONTAINER_PATH:?}" "$@"
}

## dyncontainer_import ( src_uri, dst_relpath:=<auto>, **v0! )
dyncontainer_import() {
   import_file "${DYNCONTAINER_PATH:?}" "{@}"
}
