## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _dotfiles_import_from_tarball (
##    file_uri, tarball_filename:=, **tmpdir, **dstdir
## )
##
_dotfiles_import_from_tarball() {
   <%%locals tfile v0 %>

   import_file "${tmpdir}" "${1:?}" "${2-}" || return
   tfile="${v0:?}"

   @@PROG_UNTAR@@ -C "${dstdir}/src-tmp" "${tfile}" || return
   rm -- "${tfile}" || return

   # yeah, this breaks if src-tmp contains ".<name>" files/dirs
   set -- "${dstdir}/src-tmp/"*

   if [ ${#} -eq 1 ] && [ -d "${1}" ]; then
      mv    -- "${1}" "${dstdir}/src" || return
      rmdir -- "${dstdir}/src-tmp"    || return
   else
      mv -- "${dstdir}/src-tmp" "${dstdir}/src" || return
   fi
}

## int _dotfiles_do_import__any (
##    src_type, src_uri, dstname, download_filename:=,
##    **tmproot, **dstroot, **distdir!
## )
##
_dotfiles_do_import__any() {
   <%%varcheck 1..3 %>
   <%%locals tmpdir fail %>

   dstdir="${dstroot%/}/${3}"
   tmpdir="${tmproot%/}/${3}"

   autodie mkdir -- "${dstdir}"
   printf '%s\n' "${2}" > "${dstdir}/src_uri" || die "#src_uri"

   autodie mkdir -- "${tmpdir}"
   ( _dotfiles_import_from_${1} "${2}" "${4-}"; ) && fail=0 || fail=${?}
   autodie rm -r -- "${tmpdir}"

   return ${fail}
}

## int _dotfiles_do_import__dotfiles (
##    src_type, src_uri, src_index, **tmproot, **dstroot, **need_tbt!
## )
##
_dotfiles_do_import__dotfiles() {
   <%%varcheck 1..3  %>
   <%%locals index_str dstdir %>

   index_str="$(printf '%04d' "${3}")" && [ -n "${index_str}" ] || return

   einfo "Importing dotfiles ${1} ${2##*/} (as ${index_str})"
   _dotfiles_do_import__any "${1}" "${2}" "${index_str}" || return

   if [ -e "${dstdir}/src/tbt" ]; then
      need_tbt=master
   fi
}

## int _dotfiles_do_import__tbt (
##    ref_or_version, **tmproot, **dstroot
## )
_dotfiles_do_import__tbt() {
   <%%varcheck 1 %>
   <%%locals dstdir %>

   einfo "Importing things-beyond-things ${1}"

   if [ -n "${CMDLINE_DOTFILES_TBT_SRC_URI-}" ]; then
      einfo "Using cmdline-specified src uri"

      _dotfiles_do_import__any "tarball" \
         "${CMDLINE_DOTFILES_TBT_SRC_URI}" "tbt-${1}" || return
   else
      _dotfiles_do_import__any "tarball" \
         "@@DOTFILES_TBT_SRC_SITE@@:${1}" \
         "tbt-${1}" "tbt-${1}.tar.gz" || return
   fi
}


## @autodie dotfiles_import ( dstroot, tmproot, dotfiles_src_list_file )
##
dotfiles_import() {
   <%%varcheck 1..3 %>
   <%%locals dstroot tmproot src_type src_uri CRAP index need_tbt %>

   dstroot="${1}"
   tmproot="${2}"
   autodie test -f "${3}"
   dodir "${dstroot}"
   dodir "${tmproot}"
   need_tbt=

   index=0
   while read -r src_type src_uri CRAP; do

      case "${src_type}" in
         ''|'#'*) continue ;;
      esac

      [ -z "${CRAP}" ] || die "bad dotfiles config!"

      case "${src_type}" in
         none)
            @@NOP@@
         ;;

         tarball)
            autodie _dotfiles_do_import__dotfiles \
               "${src_type}" "${src_uri}" "${index}"
            <%% inc index %>
         ;;

         *)
            die "dotfiles config: unknown file type '${src_type}' for '${src_uri}'"
         ;;
      esac

   done < "${3}" || die "Failed to read dotfiles config!"

   if [ -n "${need_tbt}" ]; then
      einfo "Downloading tbt dotfiles install scripts: ${need_tbt}"

      for src_uri in ${need_tbt}; do
         autodie _dotfiles_do_import__tbt "${src_uri}"
      done
   fi

   autodie rmdir -- "${tmproot}"

   if [ ${index} -gt 9999 ]; then
      ewarn "Imported ${index} > 9999 dotfile projects - this breaks ordered looping."
   fi
}

## @autodie _import_dotfiles_to_root (
##    dotfiles_src_list_file:=**DOTFILES_SRC_LIST_FILE, **v0!
## )
##
_import_dotfiles_to_root() {
   : ${ROOT:?}
   v0=
   [ ${#} -eq 0 ] || [ -n "${1}" ] || die "Bad usage."

   dotfiles_import \
      "${ROOT%/}/${NEWROOT_DOTFILES_REL#/}" \
      "${ISHARE_TMP:?}/dotfiles" \
      "${1:-${DOTFILES_SRC_LIST_FILE:?}}" && \
   v0="${ROOT%/}/${NEWROOT_DOTFILES_REL#/}"
}
