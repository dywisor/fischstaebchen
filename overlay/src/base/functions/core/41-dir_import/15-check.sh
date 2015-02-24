## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int import_to_dir_check_variant_supported (
##    ["-q"|"--quiet"], variant, [src], **v0!
## )
##
import_to_dir_check_variant_supported() {
   <%%retvar v0 %>
   <%%locals quiet= %>
   case "${1-}" in
      '-q'|'--quiet')        quiet=y; @@SHIFT_OR_RET@@ ;;
      '-v'|'--verbose'|'--') @@SHIFT_OR_RET@@ ;;
   esac

<%define _MSGPRE "import_to_dir variant not supported: " %>
<%define _QPRINT [ -n "${quiet}" ] || ewarn %>

   case "${1-}" in
      '')
         @@_QPRINT@@  "@@_MSGPRE@@<none specified>."
         return @@EX_USAGE@@
      ;;

      bind|bind_ro)
         v0="${1}"

         # yeah, bind-mounting files is generally supported,
         # but not by this module.
         if [ -n "${2-}" ]; then

            if [ -h "${2}" ] || [ ! -d "${2}" ]; then
               @@_QPRINT@@ "@@_MSGPRE@@: ${1}: src is not a real dir: ${2}"
               return @@EX_NOT_SUPPORTED@@
            fi

         fi
      ;;

      cp|copy)
         v0=copy

         # %src may be a symlink (gets derefenced when copying) (must exist)
         if [ -n "${2-}" ] && [ ! -d "${2}" ]; then
            @@_QPRINT@@ "@@_MSGPRE@@: ${1}: src is not a dir: ${2}"
            return @@EX_NOT_SUPPORTED@@
         fi
      ;;

      tar|untar)
         v0=untar

         # %src may be a symlink (gets dereferenced when copying) (must exist)
         if [ -n "${2-}" ] && [ ! -f "${2}" ]; then
            @@_QPRINT@@ "@@_MSGPRE@@: ${1}: src is not a file: ${2}"
            return @@EX_NOT_SUPPORTED@@
         fi
      ;;

      sym|symlink)
         v0=symlink

         if [ -n "${2-}" ] && [ ! -e "${2}" ]; then
            @@_QPRINT@@ "@@_MSGPRE@@: ${1}: src does not exist: ${2}"
            return @@EX_NOT_SUPPORTED@@
         fi
      ;;

      *)
         @@_QPRINT@@  "@@_MSGPRE@@'${1}' is unknown."
         return @@EX_NOT_SUPPORTED@@
      ;;

   esac

<%undef _QPRINT %>
<%undef _MSGPRE %>

   : ${v0:?BUG: import_to_dir_check_variant_supported did not set v0.}
   if [ -z "${quiet}" ]; then
      case "${v0}" in
         "${1}") vveinfo "import_to_dir variant '${v0}' is supported." ;;
         *) vveinfo "import_to_dir variant '${v0}' ('${1}') is supported." ;;
      esac
   fi

   return 0
}
