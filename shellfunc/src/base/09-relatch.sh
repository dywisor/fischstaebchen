## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

retlatch() {
   if "${@}"; then
      rc=0
      return 1
   else
      rc=${?}
      return 0
   fi
}

