## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _do_import_to_dir (
##    variant, name, src_relpath!e, src, dst, dst_bak:=,
##    **IMPORT_DIR_RSYNC_OPTS=%IMPORT_DIR_DEFAULT_RSYNC_OPTS
## )
##
##   %variant, %src_relpath, %src should be valid
##   (and, in case of %variant, also unaliased)
##   when calling this function
##    (check with import_to_dir_check_variant_supported())
##
_do_import_to_dir() {
   <%%locals git_uri git_checkout opts %>
   <%%locals !\
      | variant=${1:?} name=${2:?} src_relpath=${3?} !\
      | src=${4:?} dst=${5:?} dst_bak=${6-} %>

<% if PARANOID= %>
   if [ ${#} -gt 6 ]; then
      die "_do_import_to_dir() takes up to 6 args, got ${#}." @@EX_USAGE@@
   fi

   case "${variant}" in
      symlink)
         if [ -z "${src_relpath}" ]; then
            die "BUG: _do_import_to_dir(variant=${variant}) called with empty src_relpath"
         fi
      ;;
      bind|bind_ro|copy|untar|git|rsync)
         @@NOP@@
      ;;
      *)
         die "BUG: _do_import_to_dir() called with invalid variant '${variant}'"
      ;;
   esac
<% endif %>

   if [ -n "${dst_bak}" ]; then
      die "_do_import_to_dir(): dst backup not supported so far." @@EX_NOT_SUPPORTED@@
   fi

   if [ -n "${src_relpath}" ]; then
      if ! {
         case "${dst}" in
            */*) test -e "${dst%/*}/${src_relpath}" ;;
            *)   test -e "${src_relpath}" ;;
         esac
      }; then

         eerror "_do_import_to_dir() has been called with an invalid %src_relpath."
         eerror " dirname('${dst}') + '${src_relpath}' should point to an existing ${src}"
         ## and yes, the check above does not verify that.

         die "_do_import_to_dir(): usage error"
      fi
   fi

   case "${variant}" in
      symlink)
         @@DBGTRACE_FUNC@@create_symlink \
            "${src_relpath:?}" "${dst}" "${name}" || return

         return ${?}
      ;;
   esac

   @@DBGTRACE_FUNC@@_import_to_dir_prepare_dst_dir "${dst}" || return

   case "${variant}" in

      bind|bind_ro)
         @@DBGTRACE_FUNC@@xmount_${variant} "${src}" "${dst}" || return
      ;;

      copy)
         @@DBGTRACE_FUNC@@copytree "${src}" "${dst}" || return
      ;;

      untar)
         @@DBGTRACE_CMD@@@@PROG_UNTAR@@ -C "${dst}" "${src}" || return
      ;;

      git)
         if ! split_git_uri "${src}"; then
            eerror "BUG in _do_import_to_dir(): invalid git uri: ${src}"
            return @@EX_SOFTWARE@@
         fi

         ## FIXME: default upstream branch is not necessarily master
         @@DBGTRACE_FUNC@@git_repo_update \
            "${git_uri}" "${dst}" "${git_checkout:-master}" || return
      ;;

      rsync)
         opts="-r ${IMPORT_DIR_RSYNC_OPTS-${IMPORT_DIR_DEFAULT_RSYNC_OPTS?}}"
         ! __debug__ || opts="${opts} --progress --stats"

         ## copy content of src into dst
         ## so that src/A gets copied to dst/A (and not dst/src/A).
         ## to realize this, append "/" to both src and dst
         @@DBGTRACE_CMD@@rsync ${opts} "${src%/*}/" "${dst%/*}/" || return
      ;;

      *)
         die "BUG: _do_import_to_dir() has been called with an unknown variant (${variant})"
      ;;

   esac
}
