## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

dosym() {
   autodie ln -s -- "${1:?}" "${2:?}"
}

dosym_force() {
   autodie ln -s -f -- "${1:?}" "${2:?}"
}

dosym_if_nexist() {
   _test_fs_lexists "${2:?}" || autodie ln -s -- "${1:?}" "${2:?}"
}

nullsym() {
   <%%argc_loop ::: autodie ln -s -- /dev/null "${1:?}" %>
}

nullsym_force() {
   <%%argc_loop ::: autodie ln -s -f -- /dev/null "${1:?}" %>
}

nullsym_if_nexist() {
   <%%argc_loop ::: _test_fs_lexists "${1:?}" || autodie ln -s -- /dev/null "${1:?}" %>
}


## int read_symlink_nonfatal ( [opt...], filepath, **v0! )
##
read_symlink_nonfatal() {
   v0="$(readlink "${@}")" && [ -n "${v0}" ]
}

## int read_symlink_nonfatal_verbose ( [opt...], filepath, **v0! )
##
read_symlink_nonfatal_verbose() {
   ! read_symlink_nonfatal "${@}" || return 0
   ewarn "failed to resolve symlink: ${*}"
   return 1
}

## @autodie read_symlink ( [opt...], filepath, **v0! )
##
read_symlink() {
   read_symlink_nonfatal "${@}" || die "failed to resolve symlink: ${*}"
}


## int create_symlink_nonfatal ( link_target, link, name:= )
##
create_symlink_nonfatal() {
   <%%locals v0 %>
   <%%locals link_target=${1:?} link=${2:?} name=${3:-${link}} %>


   if test -h "${link}"; then
      read_symlink_nonfatal_verbose "${link}" || return

      if [ "${v0:?}" = "${link_target}" ]; then
         veinfo "Skipping creation of symlink ${name}: already set up."
         return 0
      fi

      veinfo "About to replace symlink ${name} (with a symlink to ${link_target})"
      if ! rm -- "${link}"; then
         ewarn "Failed to remove old symlink ${link}."
         return 1
      fi


   elif test -d "${link}"; then
      veinfo "About to replace dir ${name} (with a symlink to ${link_target})"
      # fails if dir not empty
      if ! rmdir -- "${link}"; then
         ewarn "Failed to remove old directory ${link} - is it empty?"
         return 1
      fi

   elif test -e "${link}"; then
      ewarn "Cannot replace non-symlink/non-dir ${link} (exists)"
      return 1

   else
      dodir_for_file_nonfatal "${link}" || return ${?}
   fi

   veinfo "Creating symlink ${link} => ${link_target}"
   ln -s -- "${link_target}" "${link}"
}

## @autodie create_symlink ( link_target, link, name:= )
##
create_symlink() { autodie create_symlink_nonfatal "${@}"; }

## @autodie create_symlink_if_nexist() ( link_target, link, name:= )
##
create_symlink_if_nexist() {
   test -h "${2:?}" || test -e "${2:?}" || create_symlink "${@}"
}


## int copy_symlink_nonfatal ( src, dst, name:= )
##
copy_symlink_nonfatal() {
   <%%varcheck 1..2 %>
   <%%local v0 %>

   read_symlink_nonfatal_verbose "${1}" && \
   create_symlink_nonfatal "${v0:?}" "${2}" "${3:-${2}}"
}

## @autodie copy_symlink ( src, dst, name:= )
##
copy_symlink() {
   <%%varcheck 1..2 %>
   <%%local v0 %>

   autodie read_symlink_nonfatal_verbose "${1}" && \
   create_symlink "${v0:?}" "${2}" "${3:-${2}}"
}
