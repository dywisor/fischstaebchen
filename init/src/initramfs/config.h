/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_CONFIG_H_
#define _INITRAMFS_CONFIG_H_

#include "../common/config.h"
#include "../common/baselayout/config.h"
#include "../zram/config.h"

#ifndef INITRAMFS_RUNDIR
#define INITRAMFS_RUNDIR  RUN_MOUNTPOINT "/initramfs"
#endif

#define GET_INITRAMFS_RUNDIR(p)  INITRAMFS_RUNDIR "/" p

#ifndef INITRAMFS_RUN_TMPDIR
#define INITRAMFS_RUN_TMPDIR  GET_INITRAMFS_RUNDIR("tmp")
#endif

#ifndef INITRAMFS_LOGFILE_PATH
/*#define INITRAMFS_LOGFILE_PATH  GET_INITRAMFS_RUNDIR("initramfs.log")*/
#define INITRAMFS_LOGFILE_PATH "/initramfs.log"
#endif

#define GET_INITRAMFS_RUN_TMPDIR(p)  INITRAMFS_RUN_TMPDIR "/" p

#ifndef INITRAMFS_RUN_CONFDIR
#define INITRAMFS_RUN_CONFDIR  GET_INITRAMFS_RUNDIR("config")
#endif

#define GET_INITRAMFS_RUN_CONFDIR(p)  INITRAMFS_RUN_CONFDIR "/" p

#ifndef INITRAMFS_RUN_HOOKDIR
#define INITRAMFS_RUN_HOOKDIR  GET_INITRAMFS_RUNDIR("hooks")
#endif

#define GET_INITRAMFS_RUN_HOOKDIR(p)  INITRAMFS_RUN_HOOKDIR "/" p

#ifndef INITRAMFS_DATADIR
#define INITRAMFS_DATADIR   "/init.d"
#endif

#define GET_INITRAMFS_DATADIR(p)   INITRAMFS_DATADIR "/" p

#ifndef INITRAMFS_HOOKDIR
#define INITRAMFS_HOOKDIR  GET_INITRAMFS_DATADIR("hooks")
#endif

#define GET_INITRAMFS_HOOKDIR(p)  INITRAMFS_HOOKDIR "/" p

#ifndef INITRAMFS_CONFDIR
#define INITRAMFS_CONFDIR  GET_INITRAMFS_DATADIR("config")
#endif

#define GET_INITRAMFS_CONFDIR(p)  INITRAMFS_CONFDIR "/" p


#ifndef LDCONFIG_EXE
#define LDCONFIG_EXE  "/sbin/ldconfig"
#endif

#ifndef LD_SO_CACHE_FILE
#define LD_SO_CACHE_FILE  "/etc/ld.so.cache"
#endif

#ifndef LD_SO_CONFIG_FILE
#define LD_SO_CONFIG_FILE  "/etc/ld.so.conf"
#endif

#endif /* _INITRAMFS_CONFIG_H_ */
