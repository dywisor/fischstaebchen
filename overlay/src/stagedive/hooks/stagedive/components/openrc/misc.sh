## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

print_vassign rc_logger YES >> "${ROOT%/}/etc/rc.conf" || \
   die "Failed to configure logging in rc.conf!"
