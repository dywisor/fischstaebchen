## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int import_file ( destdir, src_uri, dst_relpath:=, **v0! )
##
##  Imports a file using the "best" implementation available.
##
import_file() {
   import_file_verify_digest "$@"
}

## int import_file_verify_digest (
##    destdir, src_uri, dst_relpath:=, **did_verify!, **v0!
## )
##
import_file_verify_digest() {
   _import_file verify_digest "$@"
}

## int import_file_nohash ( destdir, src_uri, dst_relpath:=, **v0! )
##
import_file_nohash() {
   _import_file nohash "$@"
}

## int intercept_file_import_do_check_have_net()
##
intercept_file_import_do_check_have_net() {
   if ! ishare_has_flag have-net; then
      eerror \
         "cannot import ${uri} via ${type}: networking is not available."
      return 2
   fi
}

## int intercept_file_import_check_have_net()
##
intercept_file_import_check_have_net() {
   case "${type}" in
      local|disk|aux)
         true
      ;;

      *)
         intercept_file_import_do_check_have_net || return ${?}
      ;;
   esac

   return 0
}

## void intercept_file_import_pre_dosym(), raises die()
##
intercept_file_import_pre_dosym() {
   case "${method}" in
      local)
         [ -n "${src_filepath-}" ] || die "src_filepath is not set?"
         method=symlink
         want_chmod=n
      ;;
   esac

   return 0
}

## int intercept_file_import_pre_nocopy(), raises die()
##
intercept_file_import_pre_nocopy() {
   case "${method}" in
      local)
         [ -n "${src_filepath-}" ] || die "src_filepath is not set?"

         dst="${src_filepath}"
         return 1
      ;;
   esac

   return 0
}
