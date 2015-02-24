## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void _union_mount_add_backing_mem_branch ( mp, basename, mem_type, [size] )
##
_union_mount_add_backing_mem_branch() {
   <%%varcheck 1..3 %>
   <%%local size %>

   size="${4:--}"
   [ "${size}" != "-" ] || size="50%"

   case "${3}" in
      '-'|zram|zdisk|ztmpfs)
         veinfo "Adding zram as backing memory (size=${size}) [${2}]"

         autodie ztmpfs -m 0755 -S ${size} ${2}_zram "${1}"

      ;;
      tmpfs|mem)
         veinfo "Adding tmpfs as backing memory (size=${size}) [${2}]"

         xmount "${mp}" ${2}_mem tmpfs size=${size},mode=0755
      ;;
      *)
         die "invalid backing mem type: ${3}"
      ;;
   esac

   return 0
}

## void union_mount_add_backing_mem_branch_detached (
##    mnt_name, basename, mem_type, [size], **mp!
## )
##
union_mount_add_backing_mem_branch_detached() {
   mp="${union_mount_container}/mem/${1:?}"; shift || die

   if [ -d "${mp}" ] && is_mounted "${mp}"; then
      die "mem branch not unique: ${mp}"
      return
   fi

   _union_mount_add_backing_mem_branch "${mp}" "${@}"
}


## void union_mount_add_backing_mem_branch (
##    layer_rw_mode, mnt_name, basename, mem_type, [size], **mp!
## )
##
union_mount_add_backing_mem_branch() {
   <%%retvar mp %>
   <%%locals rw_mode< %>

   union_mount_add_backing_mem_branch_detached "${@}" && \
   _union_mount_add_branch "${rw_mode}" "${mp:?}"
}

## void union_mount_add_backing_mem ( mem_type, [size] ), raises die()
##
##  ... $$DESC$$ ...
##
##  In contrast to disk mounts, only one memory backend is supported and
##  it is always mounted and attached in rw mode.
##
##  (One user-configured memory backend - overlay[fs] may add auxiliary mounts)
##
union_mount_add_backing_mem() {
   <%%locals mp %>

   union_mount_add_backing_mem_branch rw mem "${union_name:?}" "${@}" && \
   _union_prev_branch_was_mem=y
}

union_mount_check_mountpoint_is_mem() {
   [ "${1:-/}" != " /" ] || return @@EX_USAGE@@

   awk -v "mp=${1}" \
'
BEGIN{ex=2;}

{m=0;}
($2 == mp) {m=1; ex=1;}
($1 ~ "/dev/zram[0-9]+") {ex=0;}
($3 == "tmpfs") {ex=0;}
(m) {exit;}

END{exit ex;}' /proc/self/mounts
}
