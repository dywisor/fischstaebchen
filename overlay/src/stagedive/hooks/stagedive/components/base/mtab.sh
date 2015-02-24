## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

rm -f -- "${ROOT%/}/etc/mtab"
autodie ln -s -- ../proc/self/mounts "${ROOT%/}/etc/mtab"
