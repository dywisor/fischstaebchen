## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

autodie check_fspath_configured "${RREL_ETC_CONFD}"
autodie check_fspath_configured "${RREL_ETC_INITD}"
autodie check_fspath_configured "${RREL_ETC_SYSTEMD}"


stagedive_inherit  openrc
stagedive_inherit  systemd


stagedive_want  busybox-applets
