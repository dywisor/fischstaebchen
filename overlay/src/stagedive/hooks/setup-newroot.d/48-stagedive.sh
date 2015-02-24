## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

[ "${STAGEDIVE_TYPE:-none}" != "none" ] || exit 0

autodie _stagedive_run_setup_hooks \
   "${STAGEDIVE_TYPE}" "${NEWROOT}" "${STAGEDIVE_HOOK_MASK:-none}"
