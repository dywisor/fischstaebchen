## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

profd="${ROOT%/}/etc/profile.d"

dodir "${profd}"

write_to_file "${profd}/10-set_tmpdir.sh" << EOF
if [ -z "\${USER-}" ]; then
   USER="\$(id -n -u @@NO_STDERR@@)"
fi

if [ -n "\${USER}" ] && [ -d "/tmp/users/\${USER}" ]; then
   TMPDIR="/tmp/users/\${USER}"
   export TMPDIR
fi

:
EOF
