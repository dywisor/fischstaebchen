## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_install_dotfiles_from_srcdir() {
   <%%locals need_tbt%>

   einfo "Installing ${1:?} dotfiles from ${dotfile_dir}"

   if [ -e "${srcdir}/tbt" ]; then
      veinfo "${dotfile_dir}: Found tbt install file"
      need_tbt=master

      if [ -d "${srcroot}/tbt-${need_tbt}/src" ]; then
         (
            export TBT_PRJROOT="${srcroot}/tbt-${need_tbt}/src"
            ${SHELL:-sh} "${TBT_PRJROOT}/${1}_install.sh" \
               "${srcdir}" "${dstdir}" - "${ROOT-}"
         )
         return ${?}

      else
         eerror "Cannot install dotfiles '${dotfile_dir}': tbt-${need_tbt} is missing."
         return @@EX_NOT_SUPPORTED@@
      fi

   elif [ -f "${srcdir}/${1}_install.sh" ]; then
      veinfo "${dotfile_dir}: Found ${1}_install.sh"
      ${SHELL:-sh} "${srcdir}/${1}_install.sh" \
         "${srcdir}" "${dstdir}" - "${ROOT-}"
      return ${?}
   fi

   eerror "Cannot install dotfiles '${dotfile_dir}': no supported method found."
   return @@EX_NOT_SUPPORTED@@
}

## int _install_dotfiles_nonfatal ( install_type, srcroot, dstdir )
_install_dotfiles_nonfatal() {
   <%%locals dotfile_dir dotfile_name srcdir install_type< srcroot< dstdir< fail %>
   autodie test -d "${dstdir}"

   einfo "Installing ${install_type} dotfiles to ${dstdir}"

   for srcdir in "${srcroot}/"?*"/src"; do
      if [ -d "${srcdir}" ]; then
         dotfile_dir="${srcdir%/*}"
         dotfile_name="${dotfile_dir##*/}"

         case "${dotfile_name}" in
            tbt|tbt-*)
               @@NOP@@
            ;;

            *)
               ##autodie test -d "${dstdir}"
               if _install_dotfiles_from_srcdir "${install_type:?}"; then
                  @@NOP@@
               else
                  fail=${?}
                  eerror "Failed to install dotfiles '${dotfile_dir}'"
                  return ${fail}
               fi
            ;;
         esac
      fi
   done
}
