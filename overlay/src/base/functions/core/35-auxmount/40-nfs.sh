## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int do_auxmount_nfs ( export_path, opts:=..., **v0! )
##
do_auxmount_nfs() {
   <%%locals export rel_mp %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

   export="$(strip_fspath "${1#nfs://}")"
   [ -n "${export}" ] || die "strip_fspath() is broken."
   export="${export#/}"

   export="$(printf "%s" "${export}" | sed -r -e 's,^([^/:]+)/,\1:/,')"
   [ -n "${export}" ] || die "failed to normalize export path"

   _auxmount_hash_rel_mp "${export}"

   _auxmount_do_mount "nfs/${rel_mp}" "${export}" \
      "nfs" "${2:-${AUXMOUNT_DEFAULT_NFS_OPTS:-ro,nolock}}"
}
