## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

[ "${STAGEDIVE_CFG_KEYMAP}" != "none" ] || return 0

if \
   @@NO_STDERR@@ find \
      "${ROOT%/}/usr/share/keymaps" \
         -type f -name "${STAGEDIVE_CFG_KEYMAP}.map*" | grep -q -- .
then

   print_vassign keymap "${STAGEDIVE_CFG_KEYMAP}" \
      >> "${ROOT_ETC_CONFD}/keymaps" || \
      die "Failed to configure keymap!"

else
   ewarn "Cannot set keymap - file not found"
fi
