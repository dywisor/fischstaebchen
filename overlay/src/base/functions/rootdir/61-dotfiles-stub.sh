## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## root_install_user_dotfiles ( user|dstdir_rel, ... )
##
root_install_user_dotfiles() {
   [ -n "${1-}" ] || \
      ewarn "root_install_user_dotfiles() called without user/dstdir arg."

   if [ -z "${NEWROOT_DOTFILES_REL-}" ]; then
      veinfo "Not installing user dotfiles: feature not available."
   else
      autodie _install_user_dotfiles_to_root "${@}"
   fi
}

## root_install_system_dotfiles ( ... )
##
root_install_system_dotfiles() {
   if [ -z "${NEWROOT_DOTFILES_REL-}" ]; then
      veinfo "Not installing system dotfiles: feature not available."
   else
      autodie _install_system_dotfiles_to_root "${@}"
   fi
}

## root_import_dotfiles ( ..., **v0! )
##
root_import_dotfiles() {
   if [ -z "${NEWROOT_DOTFILES_REL-}" ]; then
      v0=
      veinfo "Not importing dotfiles: feature not available."
   else
      autodie _import_dotfiles_to_root "${@}"
   fi
}
