## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if ishare_has_flag want-dotfiles-import; then
   if [ -s "${DOTFILES_SRC_LIST_FILE:?}" ]; then
      ROOT="${NEWROOT}"
      autodie _import_dotfiles_to_root
   fi
   ishare_del_flag want-dotfiles-import
else
   einfo "Dotfiles already imported - skipping."
fi
