## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

int split_git_uri(uri_checkout_str, **git_uri!, **git_checkout!) {
   <%%retvars git_uri git_checkout %>

   case "${1}" in
      *)
         return 2
      ;;
      *::*)
         git_uri="${1%::*}"
         git_checkout="${1##*::}"
      ;;
      *)
         git_uri="${1}"
         git_checkout=""
      ;;
   esac

   [ -n "${git_uri}" ]
}

int check_git_uri_valid(...) {
   <%%locals git_uri git_checkout %>
   split_git_uri "${@}"
}

int preparse_file_uri ( uri, **type!, **srcpath!, **uri! ) {
   <%%zapvars type srcpath uri %>
   <%%locals alt_srcpath %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

   alt_srcpath=""
   case "${1}" in
      /*)
         type=local
         srcpath="${1}"
      ;;

      @*|:*)
         type=aux
         srcpath="${1#[\@\:]}"
      ;;

      aux://*|file://*|disk://*|\
      http://*|https://*|ftp://*|\
      cifs://*|smb://*|nfs://*|\
      github://*|gh://*|\
      git://*|git+ssh://*|\
      rsync://*)
         type="${1%%://*}"
         srcpath="${1#*://}"
         alt_srcpath="${1}"
      ;;

      aux=*|file=*|disk=*|\
      http=*|https=*|ftp=*|\
      cifs=*|smb=*|nfs=*|\
      github=*|gh=*|\
      git=*|\
      rsync=*)
         type="${1%%=*}"
         srcpath="${1#*=}"
      ;;

      *)
         return 1
      ;;
   esac

   # 2: unalias type
   case "${type}" in
      smb)
         type=cifs
      ;;
      file)
         type=local
      ;;
      gh)
         type=github
      ;;
      git+ssh)
         type=git
      ;;
   esac

   # 3: set uri / verify srcpath (may change type non-recursive)
   case "${type}" in
      local|file)
         uri="${srcpath}"

         case "${srcpath}" in
            /*)
               return 0
            ;;
         esac

         return 2
      ;;

      aux)
         uri="@${srcpath}"
      ;;

      github)
         case "${srcpath}" in
            */*:*:*|*/*:|/*|*/|*/*/*)
               eerror "malformed github uri: ${srcpath} (should be <user>/<repo>[:<ref>])"
               return 2
            ;;

            ?*/?*:?*)
               # <user> / <repo> : <tag|branch|...>
               srcpath="@@GITHUB_ARCHIVE_SITE@@/${srcpath%:*}/tarball/${srcpath##*:}"
            ;;

            ?*/?*)
               # <user> / <repo>, branch:=master
               srcpath="@@GITHUB_ARCHIVE_SITE@@/${srcpath}/tarball/master"
            ;;

            *)
               eerror "malformed github uri: ${srcpath} (should be <user>/<repo>[:<ref>])"
               return 2
            ;;
         esac

         type="@@GITHUB_ARCHIVE_TYPE@@"
         uri="${type}://${srcpath#/}"
      ;;

      git|rsync)
         uri="${alt_srcpath:-${srcpath}}"
      ;;

      *)
         uri="${type}://${srcpath#/}"
      ;;
   esac

   case "${srcpath}" in
      ?*/?*)
         return 0
      ;;
   esac

   return 2
}

int preparse_file_uri_verbose (...) {
   <%%locals rc%>

   preparse_file_uri "${@}" || {
      rc=${?}
      case ${rc} in
         1)
            ewarn "unknown file uri: ${1:-%%%}"
         ;;
         2)
            ewarn "malformed file uri (no filename component): ${1:-%%%}"
         ;;
      esac

      return ${rc}
   }

   return 0;
}

int check_file_uri_type_dir_only(type) {
   case "${1}" in
      git)
         return 0
      ;;
      rsync)
         ## rsync is dir-only for now.
         return 0
      ;;
   esac

   return 1
}

int parse_file_uri (
   uri, **type!, **srcpath!, **uri!, **uri_basepath!, **uri_filepath!
) {
   uri_basepath=
   uri_filepath=

   preparse_file_uri_verbose "$@" || return ${?}
   ## uri is not necessarily valid after preparse_file_uri()
   ##  could reassemble it from srcpath

   case "${type}" in
      local|http*|ftp|aux|nfs|git)
         uri_basepath="${srcpath%/*}"
         uri_filepath="${srcpath##*/}"
      ;;

      disk)
         uri_basepath="${srcpath%%/*}"
         uri_filepath="${srcpath#*/}"
      ;;

      cifs)
         # breaks when share path contains space chars
         # shellcheck disable=SC2046
         set -- $(printf "%s" "${srcpath}" | \
            sed -r -e 's,^([/]*[^/]+[/][^/]+)[/](.*)$,\1 \2,') || return

         # $1: share path (host, share); $2: file path relative to the share dir
         [ $# -eq 2 ] || return
         ##[ -n "${1-}" ] && [ -n "${2-}" ] || return

         uri_basepath="${1}"
         uri_filepath="${2}"
      ;;

      *)
         die "parse_file_uri(): unhandled uri type: ${type}"
      ;;
   esac

   return 0
}

int parse_nondir_file_uri (
   uri, **type!, **srcpath!, **uri!, **uri_basepath!, **uri_filepath!
) {
   parse_file_uri "${@}" || return ${?}

   if check_file_uri_type_dir_only "${type}"; then
      ewarn "file uri references a directory: ${uri}"
      return 2
   fi

   return 0
}
