## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void _dyncontainer_init ( path, name:="dyco", initial_size:=<default> )
_dyncontainer_init() {
   xmount "${1:?}" ${2:-dyco} tmpfs \
      ${MNT_CONTAINER_BASE_OPTS:?},size=${3:-${_DYNCONTAINER_INIT_SIZE:?}m}
}

## int _dyncontainer_get_size ( path, **size! )
##
_dyncontainer_get_size() {
   <%%retvar size %>
   <%%varcheck 1 %>

   # shellcheck disable=SC2046
   set -- $( du -xms -- "${1}" ) && \
   { test "${1:--1}" -ge 0; } @@NO_STDERR@@ && \
   size="${1}"
}

## void _dyncontainer_downsize ( path, **v0! )
##
_dyncontainer_downsize() {
   <%%retvar v0 %>
   <%%locals path=${1:?} -i size new_size %>

   sync
   xremount_ro "${path}"

   _dyncontainer_get_size "${path}" || \
      die "failed to get size of dyncontainer ${path}"

   new_size=$(( size + 1 + ${DYNCONTAINER_SPARE_SIZE:-4} ))

   xremount "${path}" size=${new_size}m
   v0="${new_size}"
}
