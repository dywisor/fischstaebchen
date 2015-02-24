## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if \
   [ ! -s "${ROOT%/}/etc/resolv.conf" ] && \
   [ -f /etc/resolv.conf ]
then
   autodie copyfile /etc/resolv.conf "${ROOT%/}/etc/resolv.conf"
fi
