## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

DOTFILES_SRC_LIST_FILE="${ISHARE_CFG:?}/dotfiles_src.list"

NEWROOT_DOTFILES_REL="/factory/dotfiles"

## FIXME: remove this var, maybe rename NEWROOT_DOTFILES_REL
NEWROOT_DOTFILES="${NEWROOT%/}/${NEWROOT_DOTFILES_REL#/}"
