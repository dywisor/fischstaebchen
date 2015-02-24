## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## @funcdef import_file <type> _do_import_file__<type>(**)

_do_import_file__local() {
   copyfile_verbose "${src_filepath:?}" "${dst}"
}

<%weakdef IMPORT_FILE_WGET_TIMEOUT 10 %>
if qwhich wget; then
_do_import_file__http() {
   wget @@WGET_OPTS@@ -T @@IMPORT_FILE_WGET_TIMEOUT@@ -O "${dst}" "${uri}"
}
else
_do_import_file__http() { return @@EX_NOT_SUPPORTED@@; }
fi

_do_import_file__symlink() {
   rm -f -- "${dst}" && ln -s -- "${src_filepath:?}" "${dst}"
}


_import_file__callback() {
   <%%locals -i rc=0 %>
   ${2} || rc=$?

   if [ ${rc} -eq 1 ] || [ "${want_import}" != "y" ]; then
      veinfo "Not importing ${dst}: intercepted/success [${1:-%?%}]"

      want_import=n
      v0="${dst}"
      return 0

   elif [ ${rc} -eq 0 ]; then
      return 0

   else
      eerror "Not importing ${dst}: intercepted/fail (${rc}) [${1:-%?%}]"
      return ${rc}
   fi
}

## int _import_file ( variant, destdir, src_uri, dst_relpath:= )
##
_import_file() {
   <%%locals type srcpath %>
   <%%locals -p uri_ _ basepath filepath %>
   <%%locals dst method %>
   <%%locals -i rc %>
   <%%locals src_filepath %> # _possibly_ set _after_ F_IMPORT_FILE_INTERCEPT

   <%%locals variant< %>

   <%%locals -p want_ chmod=y import=y %>

   if retlatch _import_file_parse_uri "$@"; then
      eerror "failed to parse uri/dst" import_file
      return ${rc}
   fi

   if [ -n "${F_IMPORT_FILE_INTERCEPT-}" ]; then
      _import_file__callback early "${F_IMPORT_FILE_INTERCEPT}" || return ${?}
      [ "${want_import}" = "y" ] || return 0
   fi

   _import_file_sanity_checks || return ${?}

   if __debug__; then
      vveinfo "type=${type:-%type%}" import_file
      vveinfo "srcpath=${srcpath:-%srcpath%}" import_file
      vveinfo "uri=${uri:-%uri%}" import_file
      vveinfo "uri_basepath=${uri_basepath:-%uri_basepath%}" import_file
      vveinfo "uri_filepath=${uri_filepath:-%uri_filepath%}" import_file
      vveinfo "dst=${dst:-%dst%}" import_file
      vveinfo "method=${method:-%method%}" import_file
   fi


   src_filepath=

   case "${method}" in
      aux)
         auxmount_do_get_filepath \
            "${type}" "${uri_basepath}" "${uri_filepath}" || return

         <%%v0 src_filepath %>
         method=local

         if [ -n "${F_IMPORT_FILE_POSTMOUNT-}" ]; then
            _import_file__callback post-mount \
               "${F_IMPORT_FILE_POSTMOUNT}" || return ${?}
            [ "${want_import}" = "y" ] || return 0
         fi
      ;;
      local)
         src_filepath="${uri}"
      ;;
   esac


   case "${variant}" in
      verify_digest)
         if _import_file_verify_digest_pre; then
            method=nop
         fi
      ;;
   esac

   if [ "${method}" = "nop" ]; then
      v0="${dst}"
      return 0
   fi

   if [ -n "${F_IMPORT_FILE_PRE-}" ]; then
      _import_file__callback pre-import "${F_IMPORT_FILE_PRE}" || return ${?}
      [ "${want_import}" = "y" ] || return 0
   fi

   if retlatch _import_file_do_import; then
      eerror "failed to import ${uri:-%uri%} => ${dst:-%dst%}"
      return ${rc}
   fi

   case "${variant}" in
      verify_digest)
         _import_file_verify_digest_post || return
      ;;
   esac

   if [ -n "${F_IMPORT_FILE_DONE-}" ]; then
      ${F_IMPORT_FILE_DONE} || return ${?}
   fi

   return 0
}

_import_file_do_import() {
   <%%retvar v0 %>
   _do_import_file__${method} "${uri}" "${dst}" || return

   [ "${want_chmod:-n}" != "y" ] || chmod 0400 "${dst}"

   v0="${dst}"
   return 0
}

_import_file_parse_uri() {
   #"global" rc
   [ -n "${1-}" ] || return @@EX_USAGE@@

   if retlatch parse_file_uri "${2-}"; then
      ewarn "failed to parse file uri: ${rc}" import_file
      return ${rc}
   fi

   dst="${3-}"
   if [ -z "${dst}" ]; then
      dst="${uri_filepath##*/}"
      [ -n "${dst}" ] || return 1
   fi

   case "${type}" in
      cifs|nfs|disk|aux)
         method=aux
      ;;
      http*|ftp)
         method=http
      ;;
      *)
         method="${type}"
      ;;
   esac

   case "${dst}" in
      //*)
         dst="${dst#/}"
      ;;
      "${1%/}/"*)
         true
      ;;
      /*)
         dst="${1%/}/${dst#/}"
      ;;
      *)
         dst="${1%/}/${dst#./}"
      ;;
   esac
}

_import_file_sanity_checks() {

   if [ -z "${uri}" ]; then
      ewarn "import_file(): %uri is empty!"
      return 1

   elif [ -z "${dst}" ]; then
      ewarn "import_file(): %dst is empty"
      return 1

   elif [ "${uri}" = "${dst}" ]; then
      ewarn "import_file(): %uri equals %dst: ${uri}"
      v0="${uri}"
      return 1
   fi
}

## _import_file_do_verify_hashfile ( io_hashfunc, srcfile, hashfile )
##
_import_file_do_verify_hashfile() {
   local our_hash
   local their_hash

   if ! {
      read -r their_hash < "${3}" && [ -n "${their_hash}" ]
   }; then
      eerror "failed to read ${3}"
      return 1
   fi

   if ! {
      our_hash="$(${1} < "${2}")" && [ -n "${our_hash}" ]
   }; then
      eerror "failed to get ${1#io_} hash for ${2}!"
      return 2
   fi

   if [ "${their_hash}" != "${our_hash}" ]; then
      eerror "${1#io_} digests for ${2} do not match:"
      eerror "expected : ${their_hash}" '  -'
      eerror "got      : ${our_hash}"   '  -'
      return 3
   fi

   return 0
}

_import_file_do_verify_digest_for_file() {
   local did_verify
   local partial_fail

   [ -n "${1-}" ] || return @@EX_USAGE@@

   did_verify=
   partial_fail=0

   if [ -n "${HAVE_SHA1}" ] && [ -f "${1}.sha1" ]; then

      if _import_file_do_verify_hashfile io_sha1 "${1}" "${1}.sha1"; then
         did_verify="${did_verify} sha1"
      else
         eerror "sha1 verification failed: ${1}"
         partial_fail=1
      fi

   fi

   if [ -n "${HAVE_MD5}" ] && [ -f "${1}.md5" ]; then

      if _import_file_do_verify_hashfile io_md5sum "${1}" "${1}.md5"; then
         did_verify="${did_verify} md5"
      else
         eerror "md5 verification failed: ${1}"
         partial_fail=1
      fi

   fi


   did_verify="${did_verify# }"

   if [ -z "${did_verify}" ]; then
      if [ ${partial_fail} -eq 0 ]; then
         einfo "could not verify ${1}: no digest files available!"
         return 0
      else
         eerror "failed to verify ${1}: digest mismatch"
         return 1
      fi
   fi

   veinfo "digest verification succeeded for ${1}: ${did_verify}"
   return 0
}


_import_file_verify_digest_post() {
   if [ -n "${src_filepath}" ]; then
      _import_file_do_verify_digest_for_file "${src_filepath}" || return
   else
      ewarn "digest verification for non-aux/non-local files is not implemented."
      #return 5
   fi

   return 0
}

_import_file_verify_digest_pre() {
   [ -f "${dst}" ] || return 1

   _import_file_do_verify_digest_for_file "${dst}"
}
