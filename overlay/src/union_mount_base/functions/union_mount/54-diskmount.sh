## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @private void _union_mount_add_backing_disk (
##    "ro"|"rr"|"rw", dev, fstype, opts
## ), raises die()
##
_union_mount_add_backing_disk() {
   <%%locals mp name= mode -i k %>

   case "${2}" in
      /dev/*/*)
         :
      ;;
      /dev/*)
         name="dev-${2#/dev/}"
      ;;
      */*)
         @@NOP@@
      ;;
      LABEL=*)
         name="label-${2#LABEL=}"
      ;;
      UUID=*)
         name="uuid-${2#UUID=}"
      ;;
      PARTUUID=*)
         name="partuuid-${2#PARTUUID=}"
      ;;
      #*) @@NOP@@ ;;
   esac

   if [ -z "${name}" ]; then
      name="$( hash_fspath "${2}" )"

      if [ -n "${name}" ]; then
         name="hash-${name}"

      else
<% if FEATURE_PHASE_RETRY %>
         die "failed to get backing disk name hash" @@EX_NOT_SUPPORTED@@
<% else %>
         # this breaks when retrying a script involving union mounts
         k=0
         while [ -e "${union_mount_container}/disk/idx-${k}" ]; do
            <%%inc k %>
            [ ${k} -lt 100 ] || die "disk idx count exhausted!"
         done
         name="idx-${k}"
<% endif %>
      fi
   fi

   mp="${union_mount_container}/disk/${name}"

   case "${1}" in
      rr|ro)
         veinfo \
            "Adding readonly backing disk ${2} as ${name} (type ${3:-auto})"

         xmount "${mp}" "${2}" "${3:-auto}" "${4-}${4:+,}ro"
         mode="${1}"
      ;;
      *)
         # should be rw
         veinfo \
            "Adding writable backing disk ${2} as ${name} (type ${3:-auto})"

         xmount "${mp}" "${2}" "${3:-auto}" "${4-}${4:+,}rw"
         mode="rw"
      ;;
   esac

   if [ -d "${mp}/data" ] && [ -d "${mp}/workdir" ]; then
      # overlayfs compat: branch is ${mp}/data
      _union_mount_add_branch "${mode}" "${mp}/data"
   else
      _union_mount_add_branch "${mode}" "${mp}"
   fi
}

## void union_mount_add_backing_disk_rw ( dev, [fstype], [opts] ), raises die()
##
union_mount_add_backing_disk_rw() {
   _union_mount_add_backing_disk rw "$@"
}

## void union_mount_add_backing_disk_ro ( dev, [fstype], [opts] ), raises die()
##
union_mount_add_backing_disk_ro() {
   _union_mount_add_backing_disk ro "$@"
}

## void union_mount_add_backing_disk_rr ( dev, [fstype], [opts] ), raises die()
##
union_mount_add_backing_disk_rr() {
   _union_mount_add_backing_disk rr "$@"
}

## void union_mount_add_backing_disk_ro_or_rw (
##    word, dev, [fstype], [opts]
## ), raises die()
##
##  Adds a backing disk in rw or ro mode,
##  depending on whether %word is empty or not.
##
##  Example Usage:
##     union_mount_add_backing_disk_ro_or_rw "$NEXT_DISK" "$DISK" ...
##     union_mount_add_backing_disk_ro_or_rw "$MEM" "$NEXT_DISK" ...
##
union_mount_add_backing_disk_ro_or_rw() {
   [ -n "${1+SET}" ] || return @@EX_USAGE@@

   if [ -n "${1}" ]; then
      shift && union_mount_add_backing_disk_ro "$@"
   else
      shift && union_mount_add_backing_disk_rw "$@"
   fi
}
