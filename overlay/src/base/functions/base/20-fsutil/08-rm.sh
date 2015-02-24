## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

fs_remove() {
   if [ -h "${1}" ]; then
      autodie rm -- "${1}"

   elif [ -d "${1}" ]; then
      autodie rm -r -- "${1}"

   elif [ -e "${1}" ]; then
      autodie rm -- "${1}"
   fi
}
