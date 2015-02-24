## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

#<%if STRICT %>
#<%endif%>

_auxmount_std_symname_wrapper() {
   <%%retvars v0 v1 %>
   <%%locals func< name<=${1?} %>

   ${func} "$@" || return
   v1="${v0:?}"

   if [ "${name:-none}" != "none" ]; then
      autodie _auxmount_symlink_mount "${v1:?}" "${name}"
   fi
}

## int auxmount_disk (
##    name, disk_identifier, fs, fstype:="auto", opts:="noatime,ro", **v0!, **v1!
## )
##
auxmount_disk() {
   _auxmount_std_symname_wrapper do_auxmount_disk "${@}"
}

## int auxmount_nfs ( name, export_path, opts:=..., **v0!, **v1! )
##
auxmount_nfs() {
   _auxmount_std_symname_wrapper do_auxmount_nfs "${@}"
}

## int auxmount_cifs ( name, share_path, credentials_file, opts:=..., **v0!, **v1! )
##
auxmount_cifs() {
   _auxmount_std_symname_wrapper do_auxmount_cifs "${@}"
}

## int auxmount_aux ( name_ignored, aux_uri, **v0!, **v1 )
##
##  This is a dirty hack that allows to access aux like any other auxmount_().
##
auxmount_aux() {
   <%%retvars v0 v1 %>
   vget_auxmount "${2}" && v1="${v0:?}"
}

## int auxmount ( name, [type], uri, **v0!, **v1" )
##
auxmount() {
   <%%retvars v0 v1 %>
   <%%locals name type uri %>

   name="${1-}"
   [ -n "${2-}" ] || return @@EX_USAGE@@

   if [ -n "${3-}" ]; then
      type="${2}"
      uri="${3}"

   else
      case "${2}" in
         /*|LABEL=*|PARTUUID=*|UUID=*)
            # yes, that catches "//.../" cifs-like paths, too
            type=disk
            uri="${2}"
         ;;

         nfs=*|cifs=*|disk=*)
            type="${2%%=*}"
            uri="${2#*=}"
         ;;

         nfs://*|cifs://*|disk://*)
            type="${2%%://*}"
            uri="${2#*://}"
         ;;

         aux=*|aux://*|@*|:*)
            type=aux
            uri="${2}"
         ;;

         *://)
            eerror "unsupported auxmount uri: ${2}"
            return @@EX_NOT_SUPPORTED@@
         ;;

         *)
            type=disk
            uri="${2}"
         ;;
      esac
   fi


   auxmount_${type} "${name}" "${uri}"
}
