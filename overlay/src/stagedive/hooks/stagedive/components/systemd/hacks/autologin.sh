## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

agetty=
if root_find_prog_ignore_symlink_dirs agetty; then
   agetty="${v1}"
##elif...
fi

if [ -z "${agetty}" ]; then
   ewarn "agetty not found in ${ROOT:-???} - skipping autologin setup."
   return 0
fi


autodie test -x "${ROOT%/}/${agetty#/}"

if [ "${SYSTEMD_AUTOLOGIN_TTYS:-X}" != "none" ]; then
   for tty in ${SYSTEMD_AUTOLOGIN_TTYS:-tty1 tty2 tty3}; do

      write_to_file \
         "${ROOT_ETC_SYSTEMD_SYSTEM}/getty@${tty}.service.d/05-autologin.conf" \
<< EOF
[Service]
ExecStart=
ExecStart=-${agetty} --noclear --autologin root %I 115200 linux
EOF
   done
fi

write_to_file \
   "${ROOT_ETC_SYSTEMD_SYSTEM}/console-getty.service.d/05-autologin.conf" \
<< EOF
[Service]
ExecStart=
ExecStart=-${agetty} --noclear --autologin root -s console 115200,38400,9600
EOF
