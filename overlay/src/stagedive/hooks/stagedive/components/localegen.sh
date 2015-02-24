## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

lfile="${ROOT%/}/etc/locale.gen"

if [ -f "${lfile}" ] && grep -q -- '@protect' "${lfile}"; then
   @@NOP@@

else
# FIXME: make this configurable.
write_to_file "${lfile}" << EOF
de_DE.UTF-8 UTF-8
de_DE ISO-8859-1
de_DE ISO-8859-15
de_DE@euro ISO-8859-15
en_US ISO-8859-1
en_US.UTF-8 UTF-8
EOF
fi

chroot_root_nonfatal locale-gen @@NO_STDOUT@@ || \
   ewarn "Failed to generate locales!"
