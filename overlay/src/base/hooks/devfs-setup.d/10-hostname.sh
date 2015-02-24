## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if qwhich hostname; then
   case "$(hostname @@NO_STDERR@@)" in
      ''|'(none)')
         if [ -f /etc/hostname ]; then
            veinfo "Setting hostname from /etc/hostname"
            hostname -F /etc/hostname || @@NOP@@

         else
            name="$(hostid @@NO_STDERR@@)" && [ -n "${name}" ] || name=init
            veinfo "Setting hostname to ${name}"
            hostname "${name}" || @@NOP@@
         fi
      ;;
   esac
fi
