## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## assert constant NEWROOT
<%define _NEWROOT_REF ${NEWROOT} %>

eval_remount_status_functions  newroot      "@@_NEWROOT_REF@@"
##eval_remount_status_functions  newroot_etc  "@@_NEWROOT_REF@@/etc"
##eval_remount_status_functions  newroot_var  "@@_NEWROOT_REF@@/var"
##eval_remount_status_functions  newroot_usr  "@@_NEWROOT_REF@@/usr"
