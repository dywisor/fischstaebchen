## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

<%define _VARCHECK !\
   | : ${1:?} ${2?} ${3:?} ${4:?} !\
%>
<%define _CHECK_VARIANT_SUPPORTED !\
   | import_to_dir_check_variant_supported -- "${1}" "${3}" !\
%>
<%define _CALL_IMPORT_DIR !\
   | _do_import_to_dir "${v0:?}" "${5:-${3##*/}}" "${2}" "${3}" "${4}" !\
%>
## int import_to_dir_nonfatal ( variant, src_relpath, src, dst, name:= )
##
import_to_dir_nonfatal() {
   @@_VARCHECK@@
   <%%locals v0 %>

   @@_CHECK_VARIANT_SUPPORTED@@ || return

   @@_CALL_IMPORT_DIR@@
}

## @autodie import_to_dir ( variant, src_relpath, src, dst, name:= )
##
import_to_dir() {
   @@_VARCHECK@@
   <%%locals v0 %>

   autodie @@_CHECK_VARIANT_SUPPORTED@@ || return

   autodie @@_CALL_IMPORT_DIR@@
}
