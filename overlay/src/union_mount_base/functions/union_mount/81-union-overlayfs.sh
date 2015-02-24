## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @forward-def @stdout int union_mount_overlayfs__genscript ( aux_mnt_root, mp )
<%weakdef AUFS_OVERLAYFS_GENSCRIPT_PROG aufs-overlayfs-genscript %>

union_mount_overlayfs() {
   <%%locals mp aux_mnt_root mount_script %>

   mp="${1:-${union_mountpoint:?}}"
   union_mount_get_overlayfs_aux_mnt_root
   mount_script="${aux_mnt_root:?}/domount.sh"

   veinfo "Creating overlayfs union mount script for ${union_name:-%name%}"

   if [ -z "${union_mount_branches}" ]; then
      die "no branches configured!"
   fi

   autodie mkdir -p -- "${mount_script%/*}"
   rm -f -- "${mount_script}"

   if ! union_mount_overlayfs__genscript \
      "${aux_mnt_root}" "${mp}" > "${mount_script}"
   then
      die "failed to create union mount script for ${union_name:-%name%}"
   fi

   veinfo "Mounting overlayfs union ${union_name:-%name%} on ${mp}"

   if ! ( . "${mount_script}"; ); then
      die "failed to run union mount script for ${union_name:-%name%}"
   fi

   rm -f -- "${mount_script}"

   _union_mount_done overlayfs "${mp}"
}


union_mount_overlayfs__genscript() {
   : ${1:?<aux mount root>}
   : ${2:?<mp>}
   : ${union_mount_branches:?}

   (
      <%%vexport AOV_CMD_PREFIX autodie %>

      printf  '%s\n'  '#!/bin/sh' && \
      printf  '%s\n'  'set -e'    && \
      @@AUFS_OVERLAYFS_GENSCRIPT_PROG@@ \
         ${_union_prev_branch_was_mem:+-M} \
         -L "${1}/layers" \
         "${union_mount_branches}" \
         "${2}"
   )
}
