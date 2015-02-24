## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

ROOT="${NEWROOT}"

print_root_fstab() {
   [ "${ROOT:-/}" != "/" ] || return @@EX_NOT_SUPPORTED@@

awk -v "root=${ROOT}" \
'
BEGIN { nrl=length(root); noff=(nrl+1); }

{ m = 0; }

( $2 == root )               { m = 1; $2 = "/"; }
( $2 ~ ("^" root "/") )      { m = 1; $2 = substr($2,noff); }
( $2 ~ "^/(proc|sys|dev)" )  { m = 0; }

(m) { $4 = ($4 ",noauto"); $5 = "0"; $6 = "0"; print; }
' /proc/self/mounts
}


if ishare_has_flag stagedive-want-create-fstab; then
   f="${ROOT%/}/etc/fstab"

   if grep -- '@protect' "${f}" @@QUIET@@; then
      @@NOP@@
   else
      if test_fs_exists "${f}"; then
         if [ -s "${f}" ]; then
            autodie mv -f -- "${f}" "${f}.dist"
         else
            autodie rm -- "${f}"
         fi
      else
         dodir "${ROOT%/}/etc"
      fi

      print_root_fstab > "${f}" || \
         die "Failed to create /etc/fstab in ${ROOT:-???}."

      ishare_add_flag no-premount
   fi
fi
