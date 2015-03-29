## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

yesno() {
   case "${1-}" in
      [yY]|\
      1|\
      [yY][eE][sS]|\
      [tT][rR][uU][eE]|\
      [oO][nN]|\
      [eE][nN][aA][bB][lL][eE][dD])
         return 0
      ;;
   esac

   return 1
}
