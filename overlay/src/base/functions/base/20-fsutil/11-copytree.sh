## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if qwhich rsync; then
copytree() {
   mkdir -p -- "${2:?}" && rsync -a -- "${1}/" "${2}/"
}
else
copytree() {
   mkdir -p -- "${2:?}" && cp -dpR -- "${1}/." "${2}/."
}
fi
