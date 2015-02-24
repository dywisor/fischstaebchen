## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_auxmount_cifs_read_credentials_file() {
   <%%locals line %>

   { read -r line < "${1:-/}"; } @@NO_STDERR@@ || \
      die "cannot read credentials file: ${1:-%unset%}"

<%define _LINEVAL ""${line#*=}"" %>
   while read -r line; do
      case "${line}" in
         username=*)  username=@@_LINEVAL@@ ;;
         password=*)  password=@@_LINEVAL@@ ;;
         domain=*)    domain=@@_LINEVAL@@   ;;
      esac
   done < "${1:?}"
<%undef _LINEVAL %>
}

## int do_auxmount_cifs ( share_path, credentials_file, opts:=..., **v0! )
##
do_auxmount_cifs() {
   <%%locals share rel_mp opts username password domain %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

   share="$(strip_fspath "${1#cifs://}")"
   [ -n "${share}" ] || die "strip_fspath() is broken."

   share="$(printf "%s" "${share}" | sed 's, ,\\040,g')"
   [ -n "${share}" ] || die "failed to normalize cifs share path"
   share="//${share#/}"

   _auxmount_hash_rel_mp "${share#//}"

   # busybox mount -t cifs doesn't support "credentials="/"guest"
   <%%zapvars username password domain %>
   [ -z "${2-}" ] || autodie _auxmount_cifs_read_credentials_file "${2}"

   opts="${3:-${AUXMOUNT_DEFAULT_CIFS_OPTS:-ro}}"
   <%%vappend opts ,username=${username:-guest},password=${password:-} %>
   [ -z "${domain-}" ] || \
      <%%vappend opts ,domain=${domain} %>

   _auxmount_do_mount "cifs/${rel_mp}" "${share}" "cifs" "${2:-ro,guest}"
}
