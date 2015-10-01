## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## make install vars
INIT_INITDIR="@@INITDIR@@"
INIT_HOOKDIR="@@HOOKDIR@@"
INIT_FUNCTIONS="@@FUNCTIONS@@"
## this file:
INIT_ENVFILE="@@ENVFILE@@"

INIT_KERNEL_FIRMWARE_DIR="@@INITRAMFS_KERNEL_FIRMWARE_DIR@@"
INIT_KERNEL_MODULES_DIR="@@INITRAMFS_KERNEL_MODULES_DIR@@"


## vars usually set by PID 1
## FIXME: need to be created manually when using foreign init
: ${INITRAMFS_LOGFILE:=/initramfs.log}

: ${ISHARE:=/run/initramfs}
: ${ISHARE_HOOKS:=${ISHARE}/hooks}
: ${ISHARE_CFG:=${ISHARE}/config}
: ${ISHARE_TMP:=${ISHARE}/tmp}
: ${IENV:=${ISHARE_CFG}/env.sh}
## TMPDIR, T
## M
## NR
## RETRY

: ${IDATA:=@@DEFAULT_INITRAMFS_DATADIR@@}
: ${ICFG:=${IDATA}/config}
## FIXME: IHOOK from /init, HOOKDIR from build
##  IHOOK   := executables run by /init
##  HOOKDIR := script stubs sourced by IHOOK/<script>
##
: ${IHOOK:=${IDATA}/hooks}

: ${ICOLORS=}

: ${NEWROOT:=@@DEFAULT_NEWROOT@@}
NEWROOT_KERNEL_FIRMWARE_DIR="${NEWROOT}@@NEWROOT_KERNEL_FIRMWARE_DIR@@"
NEWROOT_KERNEL_MODULES_DIR="${NEWROOT}@@NEWROOT_KERNEL_MODULES_DIR@@"
<% if DEPRECATED_VARS=0 %>
NEWROOT_FIRMWARE_DIR="${NEWROOT_KERNEL_FIRMWARE_DIR}"
NEWROOT_MODULES_DIR="${NEWROOT_KERNEL_MODULES_DIR}"
<% endif %>
NEWROOT_CONFDIR="${NEWROOT}@@NEWROOT_CONFDIR@@"
NEWROOT_HOOKDIR="${NEWROOT}@@NEWROOT_HOOKDIR@@"

: ${SHTRUE:=y}
: ${SHFALSE:=y}

##SYSMEM
##CPUCOUNT
##KREL
##KLOCAL
##ROOT_RO={y,n}
##SHELL_ON_ERROR={y,n}

: ${DEBUG:=n}
: ${VERBOSE:=y}
: ${QUIET:=n}
: ${NO_COLOR:=n}

<%weakdef DIE_DBGFILE /DIE_REASON%>
: ${DIE_DBGFILE:=@@DIE_DBGFILE@@}


## defaults
METASCRIPT_DIR="@@METASCRIPTDIR@@"

MNT_CONTAINER_BASE_OPTS="nodev,noexec,nosuid,mode=0775,gid=6"
MNT_CONTAINER_OPTS="${MNT_CONTAINER_BASE_OPTS},size=2m"

AUXMOUNT_DEFAULT_NFS_OPTS="ro,nolock,soft"
AUXMOUNT_DEFAULT_CIFS_OPTS="ro,nocase,soft"
AUXMOUNT_MNT_ROOT=/mnt/aux
AUXMOUNT_SYM_ROOT=/mnt/aux/symbolic_names

TARBALL_FILEEXT_LIST=".tar .tar.gz .tgz .tar.xz .txz .tar.bz2 .tbz2 .tar.lzo"
SQUASHFS_FILEEXT_LIST=".squashfs .sfs"

TARBALL_FILEEXT_RE="[.](tar|tgz|txz|tbz2|tar[.](gz|xz|bz2|lzo))"
SQUASHFS_FILEEXT_RE="[.](squashfs|sfs)"

## default rsync options for import_to_dir()
##  the following options are always passed to rsync: "-r"
IMPORT_DIR_DEFAULT_RSYNC_OPTS="-lptgoDH"

##if [ -z "${ARCH-}" ]; then
##ARCH="$(uname -m @@NO_STDERR@@)"
##ALT_ARCH="${ARCH}"
##case "${ARCH}" in
##   x86_64) ALT_ARCH=amd64 ;;
##   x86)    ALT_ARCH=i686  ;;
##esac
##fi
