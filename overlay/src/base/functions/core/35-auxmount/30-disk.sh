## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _do_auxmount_disk_any (
##    rel_mp, fs, fstype:="auto", opts:="noatime,ro", **v0!
## )
##
_do_auxmount_disk_any() {
   _auxmount_do_mount "disk/${1:?}" "${2:?}" "${3:-auto}" "${4:-noatime,ro}"
}

## int do_auxmount_disk_dev ( dev, ..., **v0! )
##
do_auxmount_disk_dev() {
   <%%locals dev rel_mp %>

   case "${1-}" in
      '') return @@EX_USAGE@@ ;;
      /*) dev="${1}" ;;
       *) dev="/dev/${1#./}" ;;
   esac

   dev="$( strip_fspath "${dev}" )" && [ -n "${dev}" ] || die

   _auxmount_hash_rel_mp "${dev}" dev || die

   shift && _do_auxmount_disk_any "${rel_mp}" "${dev}" "$@"
}


## int do_auxmount_disk_by_label ( label, ..., **v0! )
##
do_auxmount_disk_by_label() {
   <%%locals rel_mp %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

   _auxmount_hash_rel_mp "${1}" label || die

   _do_auxmount_disk_any "${rel_mp}" "$@"
}

## int do_auxmount_disk_by_uuid ( uuid, ..., **v0! )
##
do_auxmount_disk_by_uuid() {
   <%%locals uuid %>
   [ -n "${1-}" ] || return @@EX_USAGE@@

   _do_auxmount_disk_any "uuid/${1}" "$@"
}

## int do_auxmount_disk (
##    disk_identifier, fs, fstype:="auto", opts:="noatime,ro", **v0!
## )
##
do_auxmount_disk() {
   <%%locals disk %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

<%define _VAL ""${1#*=}"" %>
   case "${1}" in
      LABEL=*)
         if disk="$(findfs "${1}" @@NO_STDERR@@)" && [ -n "${disk}" ]; then
            # "return $?" is overly explicit
            shift && do_auxmount_disk_dev "${disk}" "$@" || return ${?}

         else
            disk=@@_VAL@@
            shift && do_auxmount_disk_by_label "${disk}" "$@" || return ${?}
         fi
      ;;

      UUID=*|PARTUUID=*)
         if disk="$(findfs "${1}" @@NO_STDERR@@)" && [ -n "${disk}" ]; then
            shift && do_auxmount_disk_dev "${disk}" "$@" || return ${?}

         else
            disk=@@_VAL@@
            shift && do_auxmount_disk_by_uuid "${disk}" "$@" || return ${?}
         fi
      ;;

      *)
         do_auxmount_disk_dev "$@" || return ${?}
      ;;
   esac
<%undef _VAL %>

   return 0
}
