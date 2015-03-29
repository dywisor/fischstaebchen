## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int copyfile ( src, dst )
##
copyfile() { cp -L -- "${1:?}" "${2:?}"; }

## int copyfile_verbose ( src, dst )
##
if __quiet__; then
## quiet mode disables verbose behavior
copyfile_verbose() { copyfile "$@"; }

elif qwhich rsync; then
## copy with fancy progress meter (rsync)
copyfile_verbose() { rsync --progress -LS -- "${1:?}" "${2:?}"; }

elif qwhich pv; then
## copy with fancy progress meter (pv)
copyfile_verbose() { pv "${1:?}" > "${2:?}"; }

else
copyfile_verbose() { cp -vL -- "$@"; }
fi
