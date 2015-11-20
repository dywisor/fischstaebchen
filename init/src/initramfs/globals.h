/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_GLOBALS_H_
#define _INITRAMFS_GLOBALS_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>

#include "../common/message.h"
#include "../common/data_types/dynarray.h"
#include "../common/decision_bool.h"
#include "../common/fs/mount_config.h"

#include "config.h"


enum {
   INITRAMFS_DOSHELL_DISABLE         = 0x00,
   INITRAMFS_DOSHELL_ONERROR         = 0x01,
   INITRAMFS_DOSHELL_ONCE            = 0x02,
   INITRAMFS_DOSHELL_ONCE_PRESWITCH  = 0x04,
   INITRAMFS_DOSHELL_LOOP            = 0x08,

   INITRAMFS_DOSHELL__MAX            = 0x10
};

enum {
   INITRAMFS_HAVEMNT_TMP = 0x1,


   INITRAMFS_HAVEMNT__LAST
};


struct initramfs_globals_type {
   DBOOL_TYPE              root_ro_status;
   DBOOL_TYPE              want_newroot_mount;
   DBOOL_TYPE              want_newroot_usr_mount;
   unsigned                rootfind_delay;
   unsigned                doshell;
   unsigned                premount_status;
   int                     have_network;

   int                     want_debug;
   int                     want_verbose;
   int                     want_quiet;

   char*                   real_init;
   const char**            shell;
   struct dynarray*        cmdline_argv;
   int                     failed_to_read_cmdline;

   char*                   kernel_release_str;
   const char*             kernel_localver_str;
   int                     num_cpu;
   size_t                  memsize_m;
   char*                   zram_autoswap_size_spec;

   struct dynarray*        load_modules;
   struct dynarray*        rootfind_devices; /* r/o so far */
   struct mount_config*    newroot_mount;
   struct mount_config*    newroot_usr_mount;

   FILE*                   logfile_stream;
};

extern struct initramfs_globals_type* initramfs_globals;

int  _initramfs_globals_init     (struct initramfs_globals_type*  const g);
void _initramfs_globals_free     (struct initramfs_globals_type*  const g);
void _initramfs_globals_free_ptr (struct initramfs_globals_type** const g);

int  initramfs_globals_init (void);
void initramfs_globals_free (void);


void _initramfs_globals_free_real_init     (struct initramfs_globals_type* const g);
void _initramfs_globals_free_zram_autoswap (struct initramfs_globals_type* const g);

void initramfs_globals_free_modprobe      (void);
void initramfs_globals_free_real_init     (void);
void initramfs_globals_free_zram_autoswap (void);

int  initramfs_init_all_globals (void);
void initramfs_free_all_globals (void);

int initramfs_globals_init_if_necessary (void);

void initramfs_globals_balance_verbosity (void);
void initramfs_globals_set_default_verbosity (void);
void initramfs_globals_set_quiet (void);
void initramfs_globals_set_verbose (void);
void initramfs_globals_set_debug (void);

int  initramfs_globals_open_logfile  (void);
void initramfs_globals_close_logfile (void);


/*#define _initramfs_check_have_log()  0*/

#define _initramfs_check_have_log()  \
   ( \
      ( initramfs_globals != NULL ) && \
      ( initramfs_globals->logfile_stream != NULL ) \
   )

#define _initramfs_have_log_do(func,...) \
   do { \
      if ( _initramfs_check_have_log() ) { \
         if ( func ( __VA_ARGS__ ) ) {}; \
      } \
   } while (0)



#define initramfs_flush_log()  \
   _initramfs_have_log_do ( fflush, initramfs_globals->logfile_stream )


#define initramfs_logf(fmt,...)  \
   _initramfs_have_log_do ( \
      fprintf, initramfs_globals->logfile_stream, fmt, __VA_ARGS__ )

#define initramfs_log(message)  \
   _initramfs_have_log_do ( \
      fprintf, initramfs_globals->logfile_stream, "%s\n", message )

#define _initramfs_printf_logf(logf_hdr,fmt,fmt_end,...) \
   initramfs_logf ( "[init] %s: " fmt fmt_end, logf_hdr, __VA_ARGS__ )

#define _initramfs_printf(flag,func,logf_hdr,hdr,fmt,fmt_end,...) \
   do { \
      if ( flag != 0 ) { \
         func ( hdr, fmt, fmt_end, __VA_ARGS__ ); \
         _initramfs_printf_logf ( logf_hdr, fmt, fmt_end, __VA_ARGS__ ); \
      } \
   } while(0)

#define _initramfs_printf_ifnot(flag,...) \
   _initramfs_printf((!flag),__VA_ARGS__)

#define initramfs_debugf(...) \
   _initramfs_printf ( \
      initramfs_globals->want_debug, printf_debug, "debug", __VA_ARGS__ )

#define initramfs_debug(...)   initramfs_debugf ( NULL, __VA_ARGS__ )

#define initramfs_vinfof(...) \
   _initramfs_printf ( \
      initramfs_globals->want_verbose, printf_message, "vinfo", __VA_ARGS__ )

#define initramfs_vinfo(...)   initramfs_vinfof ( NULL, __VA_ARGS__ )

#define initramfs_infof(...) \
   _initramfs_printf_ifnot ( \
      initramfs_globals->want_quiet, printf_message, "info", __VA_ARGS__ )

#define initramfs_info(...)  initramfs_infof ( NULL, __VA_ARGS__ )

#define initramfs_warnf(...)  \
   _initramfs_printf ( 1, printf_warning, "warning", __VA_ARGS__ )
#define initramfs_warn(...)   initramfs_warnf(NULL,__VA_ARGS__)

#define initramfs_errf(...)   \
   _initramfs_printf ( 1, printf_error, "error", __VA_ARGS__ )
#define initramfs_err(...)    initramfs_errf(NULL,__VA_ARGS__)


#endif /* _INITRAMFS_GLOBALS_H_ */
