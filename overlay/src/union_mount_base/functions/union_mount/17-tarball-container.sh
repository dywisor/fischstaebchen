## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void union_mount_get_tarball_container()
##
union_mount_get_tarball_container() {
   if [ -n "${union_tarball_container}" ]; then
      @@NOP@@

   elif is_mounted /tmp; then
      #assert writable /tmp
      union_tarball_container="/tmp/tarball-container/${union_name:?}"
      autodie mkdir -p -- "${union_tarball_container}"

   else
      die "union_mount, get_tarball_container(): /tmp is not mounted"
   fi
}

## void union_mount_finalize_tarball_container()
##
union_mount_finalize_tarball_container() {
   [ -n "${union_tarball_container}" ] || return 0

   autodie find "${union_tarball_container}/" -type f -delete
   autodie rmdir -- "${union_tarball_container}"
   union_tarball_container=
}

_union_mount_intercept_tarball_file_import_early() {
   intercept_file_import_pre_dosym
   intercept_file_import_check_have_net
}

_union_mount_intercept_tarball_file_import_postmount() {
   [ -n "${src_filepath-}" ] || die "src_filepath is not set?"
   method=symlink
   want_chmod=n
}

## int union_mount_tarball_container_get_file ( file_uri, **v0! )
##
union_mount_tarball_container_get_file() {
   <%%retvar v0 %>
   <%%locals v1 -p F_IMPORT_FILE_ INTERCEPT POSTMOUNT %>

   F_IMPORT_FILE_INTERCEPT=_union_mount_intercept_tarball_file_import_early
   F_IMPORT_FILE_POSTMOUNT=_union_mount_intercept_tarball_file_import_postmount

   union_mount_get_tarball_container && \
   import_file "${union_tarball_container}" "${1:?}"
}

## int union_mount_get_tarball ( src_uri, **tarball_file! )
##
union_mount_get_tarball() {
   tarball_file=
   local v0

   union_mount_tarball_container_get_file "${@}" && tarball_file="${v0:?}"
}
