## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## void union_mount_zap_vars()
##
##  Resets all union-mount related vars.
##  (Does not reset globals like _UNION_MOUNT_FSTYPE).
##
union_mount_zap_vars() {
   union_name=
   <%%zapvars -p union_sfs_file_     _ srcpath %>
   <%%zapvars -p union_sfs_overlay_  files -i file_index_next %>
   union_src_root=
   union_mountpoint=
   <%%zapvars -p union_mount_        container branches %>
   union_sfs_container=
   _union_sfs_container_path=
   union_tarball_container=
   <%%zapvars -p union_mount_        import_all rotate_files %>
   _union_last_import_filepath=
   _union_prev_branch_was_mem=

   # set after mounting the union:
   union_mount_fstype=

   return 0;
}

## void union_mount_set_import_all ( value )
##
union_mount_set_import_all() {
   if [ "${1:-n}" = "y" ]; then
      union_mount_import_all=y
   else
      union_mount_import_all=n
   fi
}

## void union_mount_enable_import_all()
##
union_mount_enable_import_all() {
   union_mount_import_all=y
}

## void union_mount_disable_import_all()
##
union_mount_disable_import_all() {
   union_mount_import_all=n
}


## void union_mount_set_container_path ( fspath )
##
##  Sets the mount container path for the currently configured union mount.
##
union_mount_set_container_path() {
   veinfo "Using union container ${1}"
   union_mount_container="${1}"
   _union_sfs_container_path="${union_mount_container}/sfs-container"
}

## void union_mount_get_overlayfs_aux_mnt_root ( **aux_mnt_root! )
##
union_mount_get_overlayfs_aux_mnt_root() {
   <%%retvar aux_mnt_root %>

   if [ -z "${union_mount_container-}" ]; then
      die \
"union_mount_get_overlayfs_aux_mnt_root(): \
union_mount_container is not set."
   fi

   aux_mnt_root="${union_mount_container:?}/overlayfs_aux"
}


union_mount_mark_prev_branch_as_mem() {
   _union_prev_branch_was_mem=y
}

## @private void _union_mount_add_branch ( "rw"|"ro"|"rr", branch_path )
##
##  Adds a read-write/readonly/real-readonly path to the top of the union
##  branch list.
##
_union_mount_add_branch() {
   _union_prev_branch_was_mem=
   if [ -z "${union_mount_branches}" ]; then
      union_mount_branches="${2:?}=${1:?}"
   else
      union_mount_branches="${2:?}=${1:?}:${union_mount_branches}"
   fi
}

## @private void _union_mount_append_branch ( "rw"|"ro"|"rr", branch_path )
##
##  Appends a read-write/readonly/real-readonly path to the union branch list.
##
_union_mount_append_branch() {
   _union_prev_branch_was_mem=
   if [ -z "${union_mount_branches}" ]; then
      union_mount_branches="${2:?}=${1:?}"
   else
      union_mount_branches="${union_mount_branches}:${2:?}=${1:?}"
   fi
}

## int union_mount_add_branch ( "rw"|"ro"|"rr", *branch_path )
##
##  Adds zero or more paths to the top of the union branch list.
##  The first parameter specifies the writably of the branches and
##  must be either "rw" (read-write), "ro" (readonly) or "rr" (real readonly,
##  e.g. squashfs).
##
union_mount_add_branch() {
   <%%local mode %>

   [ -n "${1-}" ] || return @@EX_USAGE@@

   mode="${1}"; shift
   case "${mode}" in
      '')
         return @@EX_USAGE@@
      ;;
      ro|rr|rw)
         @@NOP@@
      ;;
      *)
         die "expected ro/rr/rw, got ${mode}" @@EX_USAGE@@
      ;;
   esac

   while [ $# -gt 0 ]; do
      [ -d "${1}" ] || die "dir does not exist: ${1}"
      _union_mount_add_branch "${mode}" "${1}"
      shift
   done

   return 0
}

## @private void _prefix_path_with_union_src_root ( fspath, **v0! )
##
##  Prefixes a filesystem path with %union_src_root.
##  Handles already-prefixed paths, double slash "//" may be used to escape
##  "/"-absolute paths.
##
##  Stores the resulting path in %v0.
##
_prefix_path_with_union_src_root() {
   prefix_fspath "${union_src_root}" "${@}" || die
}

## @private void _union_sfs_overlay_files_append ( *paths )
##
##  Appends one or more paths to the list of sfs overlay files.
##
_union_sfs_overlay_files_append() {
   if [ -n "${union_sfs_overlay_files-}" ]; then
      union_sfs_overlay_files="${union_sfs_overlay_files} ${*}"
   else
      union_sfs_overlay_files="${*}"
   fi
}

## void _union_mount_inc_overlay_index()
_union_mount_get_and_inc_overlay_index() {
   <%%inc union_sfs_overlay_file_index_next %>
}
