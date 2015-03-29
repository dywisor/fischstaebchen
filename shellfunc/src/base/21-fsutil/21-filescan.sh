## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int do_filescan_stdout ( *argv )
do_filescan_stdout() { @@PROG_FILESCAN@@ "${@}"; }

## int do_filescan ( *argv, **v0! )
do_filescan() {
   v0="$( @@PROG_FILESCAN@@ "${@}" )" && [ -n "${v0}" ]
}


## int locate_file_nonfatal ( <suffix_list>, *basepath, **v0! )
##
locate_file_nonfatal() {
   <%%retvar v0 %>
   <%%locals filescan_suffix_list< %>

   veinfo "Trying to locate any-of: ${*} (${filescan_suffix_list})"
   if do_filescan   -f -- "${@}" -- ${filescan_suffix_list}; then
      veinfo "Found file ${v0}"
      return 0
   else
      veinfo "locate_file_nonfatal(): no file found!"
      return 1
   fi
}

## @autodie void locate_file ( <suffix_list>, *basepath, **v0! )
##
locate_file() {
   autodie locate_file_nonfatal "${@}"
}

## int locate_tarball_file_nonfatal ( *basepath, **v0 ! )
##
locate_tarball_file_nonfatal() {
   locate_file_nonfatal "${TARBALL_FILEEXT_LIST}" "${@}"
}

## @autodie void locate_tarball_file ( *basepath, **v0 ! )
##
locate_tarball_file() {
   autodie locate_file_nonfatal "${TARBALL_FILEEXT_LIST}" "${@}"
}

## int locate_squashfs_file_nonfatal ( *basepath, **v0 ! )
##
locate_squashfs_file_nonfatal() {
   locate_file_nonfatal "${SQUASHFS_FILEEXT_LIST}" "${@}"
}

## @autodie void locate_squashfs_file ( *basepath, **v0 ! )
##
locate_squashfs_file() {
   autodie locate_file_nonfatal "${SQUASHFS_FILEEXT_LIST}" "${@}"
}
