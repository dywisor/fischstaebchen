/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_CONFIG_H_
#define _COMMON_CONFIG_H_

#define __get_header(h) #h
#define _get_header(h)  __get_header(h)

#ifdef LOCALCONFIG
#define __LOCALCONFIG _get_header(LOCALCONFIG)
#include __LOCALCONFIG
#endif

#undef _get_header
#undef __get_header

/* feature selection */
#ifdef DISABLE_BUSYBOX
#undef  ENABLE_BUSYBOX
#define ENABLE_BUSYBOX 0
#endif

#ifndef ENABLE_BUSYBOX
#define ENABLE_BUSYBOX 1
#endif

#ifndef BUSYBOX_EXE
#define BUSYBOX_EXE  "/bin/busybox"
#endif

#if ENABLE_BUSYBOX
#define BUSYBOX_EXE_OR(prog)  BUSYBOX_EXE
#else
#define BUSYBOX_EXE_OR(prog)  prog
#endif

#define BUSYBOX_FPROGV_OR(name,prog)  BUSYBOX_EXE_OR(prog), name
#define BUSYBOX_PROGV_OR(name)        BUSYBOX_EXE_OR(name), name

/* busybox' findfs does not understand PARTUUID= (as of Jan 2015),
 * so you may specify a FINDFS_EXE (from util-linux) here
 * */
#ifdef FINDFS_EXE
#define FINDFS_PROGV  FINDFS_EXE, FINDFS_EXE
#else
#define FINDFS_PROGV  BUSYBOX_PROGV_OR("findfs")
#endif


/* devfs config */
#ifndef DEVFS_PATH
#define DEVFS_PATH  "/dev"
#endif

#define GET_DEVFS_PATH(p)  DEVFS_PATH p

#ifndef DEVNULL_FSPATH
#define DEVNULL_FSPATH  DEVFS_PATH "/null"
#endif

#ifndef DEVFS_TTY_GID
#define DEVFS_TTY_GID  5
#endif

#ifndef DEVFS_DISK_GID
#define DEVFS_DISK_GID  6
#endif

#ifndef DEVFS_CDROM_GID
#define DEVFS_CDROM_GID  0
#endif

/* /proc */
#ifndef PROC_PATH
#define PROC_PATH  "/proc"
#endif

#define GET_PROC_PATH(p)  PROC_PATH p

/* /sys */
#ifndef SYSFS_PATH
#define SYSFS_PATH  "/sys"
#endif

#define GET_SYSFS_PATH(p)  SYSFS_PATH p

/* shell interaction (defaults for exporting bool vars) */
#ifndef SHELL_STR_TRUE
#define SHELL_STR_TRUE "y"
#endif

#ifndef SHELL_STR_FALSE
#define SHELL_STR_FALSE "n"
#endif


/* tmpfs mount handling */
#ifndef TMPFS_ENABLE_ADDITIONAL_SIZE_SUFFIXES
#define TMPFS_ENABLE_ADDITIONAL_SIZE_SUFFIXES  1
#endif



#ifdef POST_LOCALCONFIG
#include #POST_LOCALCONFIG
#endif

enum {
   COMMON_CONFIG_H__NOT_EMPTY
};

#endif
