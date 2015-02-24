## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_install_system_dotfiles_to_root() {
   if [ ! -d "${ROOT%/}/${NEWROOT_DOTFILES_REL#/}" ]; then
      veinfo "Not installing dotfiles: none available."
      return 0
   fi

   autodie _install_dotfiles_nonfatal system \
      "${ROOT%/}/${NEWROOT_DOTFILES_REL#/}" "${ROOT}"
}
