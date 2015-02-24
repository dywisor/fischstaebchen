## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _auxmount_symlink_mount ( mp, name, **v0! )
##
_auxmount_symlink_mount() {
   <%%retvar v0 %>
   <%%locals ltarget %>

   autodie mkdir -p -- "${AUXMOUNT_SYM_ROOT}"

   v0="${AUXMOUNT_SYM_ROOT}/${2:?}"

   ! ln -s -- "${1:?}" "${v0}" @@QUIET@@ || return 0

   if \
      ltarget="$(readlink -f "${v0}" @@NO_STDERR@@)" && \
      [ "${ltarget}" = "${1}" ]
   then
      return 0
   fi

   v0=
   return 1
}

## int drop_auxmount_symlink ( name )
##
drop_auxmount_symlink() {
   rm -f -- "${AUXMOUNT_SYM_ROOT:?}/${1:?}"
}

## int get_auxmount ( name, **v0! )
##
get_auxmount() {
   v0="${AUXMOUNT_SYM_ROOT:?}/${1:?}"
   [ -d "${v0}/." ]
}

vget_auxmount() {
   <%%locals name %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

   case "${1-}" in
      '')
         return @@EX_USAGE@@
      ;;

      aux=*)
         name="${1#*=}"
      ;;
      aux://*)
         name="${1#*://}"
      ;;

      *)
         name="${1#[\@\:]}"
      ;;
   esac

   [ -n "${name}" ] || return @@EX_USAGE@@

   if get_auxmount "${name}"; then
      return 0
   else
      eerror "auxmount does not exist: ${name}"
      return 1
   fi
}


## int _auxmount_do_mount ( rel_mp, fs, fstype, opts, **v0! )
##
_auxmount_do_mount() {
   <%%locals -i rc %>

   v0="${AUXMOUNT_MNT_ROOT}/${1:?}"

   if is_mounted "${v0}"; then
      return 0
   fi

   autodie mkdir -p -- "${v0}"

   # redirect stdin (suppress password prompts)
   if \
      @@NO_STDIN@@ mount -t "${3:?}" -o "${4:?}" "${2:?}" "${v0}"
   then
      return 0
   else
      rc=${?}
      rmdir -- "${v0}" @@QUIET@@
      return ${rc}
   fi
}

## void _auxmount_hash_rel_mp ( mp_identifier, dirprefix, **rel_mp! )
##
_auxmount_hash_rel_mp() {
   <%%locals hash %>

   # (try to) hash the relative mount path so that mounts don't overlap
   if hash_str "${1}"; then
      rel_mp="${2-}${2:+-}hash/${hash}"
   else
      rel_mp="${2-}${2:+-}path/${1#/}"
   fi
}
