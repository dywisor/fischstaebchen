## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

print_vassign clean_tmp_dirs /tmp/.bootmisc \
   >> "${ROOT_ETC_CONFD}/bootmisc" || die "Failed to configure bootmisc!"
