## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## _install_user_dotfiles_to_root ( user|dstdir_rel )
##
##  Do not call this function directly,
##  use root_install_user_dotfiles() instead.
##
_install_user_dotfiles_to_root() {
   : ${1:?}
   <%% local dstdir dstdir_rel v0 v1 %>

   if [ ! -d "${ROOT%/}/${NEWROOT_DOTFILES_REL#/}" ]; then
      veinfo "Not installing dotfiles: none available."
      return 0
   fi

   dstdir=
   dstdir_rel=
   case "${1}" in
      @|@*/*|*/)
         die "bad dotfiles dstdir: ${1}"
      ;;

      @*)
         autodie root_get_home_must_exist "${1#@}"
         dstdir_rel="${v1:?}"
         dstdir="${v0:?}"
         einfo "Installing dotfiles for user ${1#@} (in <root>)"
      ;;

      /*)
         dstdir_rel="${1}"
         dstdir="${ROOT%/}${1}"
         einfo "Installing dotfiles to <root>${dstdir_rel}"
      ;;

      *)
         die "bad dotfiles dstdir: ${1}"
      ;;
   esac

   if [ -d "${dstdir}" ]; then
      veinfo "dstdir = ${dstdir}"
      autodie _install_dotfiles_nonfatal user \
         "${ROOT%/}/${NEWROOT_DOTFILES_REL#/}" "${dstdir}"
   else
      die "dotfiles dstdir does not exist: ${dstdir}"
   fi
}
