## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int ishare_has_flag ( name )
##  checks whether the given flag exists
ishare_has_flag() {
   test -e "${ISHARE_CFG}/${1}"
}

## int ishare_add_flag ( name )
##  Enables a flag file.
ishare_add_flag() {
   touch -- "${ISHARE_CFG}/${1}"
}

## int ishare_del_flag ( name )
##  Disables a flag file.
ishare_del_flag() {
   rm -f -- "${ISHARE_CFG}/${1}"
}

## int ishare_set_flag ( name, shbool )
##  Enables/Disables a flag file depending on the given shbool.
ishare_set_flag() {
   if yesno "${2:-X}"; then
      ishare_add_flag "${1}"
   else
      ishare_del_flag "${1}"
   fi
}

## ~int ishare_unless_flag_do ( name, *cmdv )
##  Calls *cmdv if the given flag does not exist.
ishare_unless_flag_do() {
   ! ishare_has_flag "${1:?}" || return 0
   @@SHIFT_OR_RET@@
   "${@}"
}

## ishare_if_flag_do ( name, *cmdv )
##  Calls *cmdv if then flag exists.
ishare_if_flag_do() {
   ishare_has_flag "${1:?}" || return 0
   @@SHIFT_OR_RET@@
   "${@}"
}
