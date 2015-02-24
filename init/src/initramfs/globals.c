/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/mount.h>

#include "globals.h"
#include "config.h"

#include "../common/mac.h"
#include "../common/message.h"
#include "../common/dynarray.h"
#include "../common/fs/mount_config.h"
#include "../common/misc/sysinfo.h"
#include "../zram/globals.h"

struct initramfs_globals_type* initramfs_globals = NULL;

static void _initramfs_globals_balance_verbosity (
   struct initramfs_globals_type* const g
) {
   if ( g->want_quiet ) {
      g->want_debug   = 0;
      g->want_verbose = 0;

   } else if ( g->want_verbose ) {

   } else if ( g->want_debug ) {
      g->want_verbose = 1;
   }

   MSG_DBGSTREAM  = g->want_debug   ? stderr : NULL;
   MSG_DEFSTREAM  = g->want_verbose ? stdout : NULL;
   MSG_ERRSTREAM  = stderr;
   MSG_WARNSTREAM = stderr;
}

static void _initramfs_globals_set_default_verbosity (
   struct initramfs_globals_type* const g
) {
   g->want_quiet   = 0;
   g->want_debug   = 0;
   g->want_verbose = 0;
}

static void _initramfs_globals_set_quiet (
   struct initramfs_globals_type* const g
) {
   g->want_quiet   = 1;
   g->want_debug   = 0;
   g->want_verbose = 0;
}

static void _initramfs_globals_set_verbose (
   struct initramfs_globals_type* const g
) {
   g->want_quiet   = 0;
   /*g->want_debug   = KEEP;*/
   g->want_verbose = 1;
}

static void _initramfs_globals_set_debug (
   struct initramfs_globals_type* const g
) {
   g->want_quiet   = 0;
   g->want_debug   = 1;
   g->want_verbose = 1;
}

int initramfs_init_all_globals (void) {
   message_set_defaults();
   /*
    * initially, the console type is unknown => disable colored output
    *
    * can be reset after mounting basemounts / before determining consoletype
    */
   MSG_NOCOLOR = 1;

   if ( initramfs_globals_init_if_necessary() != 0 ) { return -1; }

   if ( zram_globals == NULL ) {
      if ( zram_init_globals() != 0 ) { return -1; }
   }

   return 0;
}

void initramfs_free_all_globals (void) {
   zram_free_globals();
   initramfs_globals_free();
}


void initramfs_globals_balance_verbosity (void) {
   _initramfs_globals_balance_verbosity ( initramfs_globals );
}

void initramfs_globals_set_default_verbosity (void) {
   _initramfs_globals_set_default_verbosity ( initramfs_globals );
}

void initramfs_globals_set_quiet (void) {
   _initramfs_globals_set_quiet ( initramfs_globals );
}

void initramfs_globals_set_verbose (void) {
   _initramfs_globals_set_verbose ( initramfs_globals );
}

void initramfs_globals_set_debug (void) {
   _initramfs_globals_set_debug ( initramfs_globals );
}


int initramfs_globals_init_if_necessary (void) {
   if ( initramfs_globals != NULL ) {
      return 0;
   } else {
      return initramfs_globals_init();
   }
}

int initramfs_globals_init (void) {
   if ( initramfs_globals != NULL ) {
      _initramfs_globals_free ( initramfs_globals );
   } else {
      initramfs_globals = malloc ( sizeof *initramfs_globals );
      if ( initramfs_globals == NULL ) { return -1; }
   }

   if ( _initramfs_globals_init ( initramfs_globals ) != 0 ) {
      x_free ( initramfs_globals );
      return -1;
   }

   return 0;
}

void initramfs_globals_free (void) {
   _initramfs_globals_free_ptr ( &initramfs_globals );
}

void _initramfs_globals_free_ptr ( struct initramfs_globals_type** const g ) {
   if ( g != NULL ) {
      _initramfs_globals_free ( *g );
      x_free ( *g );
   }
}

static void _initramfs_free_str_ptr ( char** const pdata ) {
   x_free ( *pdata );
}

void _initramfs_globals_free_real_init (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   _initramfs_free_str_ptr ( &(g->real_init) );
}

static void _initramfs_globals_free_shell (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   x_free ( g->shell );
}

void _initramfs_globals_free_zram_autoswap (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   _initramfs_free_str_ptr ( &(g->zram_autoswap_size_spec) );
}

static void _initramfs_globals_free_newroot_mount (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   mount_config_ptr_free ( &(g->newroot_mount) );
}

static void _initramfs_globals_free_newroot_usr_mount (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   mount_config_ptr_free ( &(g->newroot_usr_mount) );
}

static void _initramfs_free_dynarray_ptr ( struct dynarray** const pp_darr ) {
   if ( *pp_darr != NULL ) {
      dynarray_free ( *pp_darr );
      x_free ( *pp_darr );
   }
}

static void _initramfs_globals_free_rootfind_devices (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   _initramfs_free_dynarray_ptr ( &(g->rootfind_devices) );
}

static void _initramfs_globals_free_modprobe (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   _initramfs_free_dynarray_ptr ( &(g->load_modules) );
}

static void _initramfs_globals_free_cmdline_argv (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   _initramfs_free_dynarray_ptr ( &(g->cmdline_argv) );
}

static void _initramfs_globals_close_logfile_stream (
   struct initramfs_globals_type* const g
) {
   /* assert g != NULL */
   if ( g->logfile_stream != NULL ) {
      if ( fflush ( g->logfile_stream ) ) {}
      if ( fclose ( g->logfile_stream ) ) {}
      g->logfile_stream = NULL;
   }
}

void _initramfs_globals_free ( struct initramfs_globals_type* const g ) {
   if ( g != NULL ) {
      _initramfs_globals_free_modprobe ( g );
      _initramfs_globals_free_rootfind_devices ( g );
      _initramfs_globals_free_real_init ( g );
      _initramfs_globals_free_shell ( g );
      _initramfs_globals_free_cmdline_argv ( g );
      _initramfs_globals_free_zram_autoswap ( g );
      _initramfs_globals_free_newroot_mount ( g );
      _initramfs_globals_free_newroot_usr_mount ( g );
      _initramfs_globals_close_logfile_stream ( g );
   }
}

static int _initramfs_globals_init_data_structs (
   struct initramfs_globals_type* const g
) {
   static const size_t SHELLV_SIZE;

   size_t k;

   g->shell = malloc ( SHELLV_SIZE * sizeof *(g->shell) );
   if ( g->shell == NULL ) { return -1; }
   for ( k = 0; k < SHELLV_SIZE; k++ ) { (g->shell) [k] = NULL; }

   g->cmdline_argv = new_dynarray ( 10 );
   if ( g->cmdline_argv == NULL ) { return -1; }

   g->load_modules = new_dynarray ( 0 );
   if ( g->load_modules == NULL ) { return -1; }

   MOUNT_CONFIG_INIT_OR_RETFAIL (
      g->newroot_mount,
      NULL,
      NEWROOT_MOUNTPOINT,
      "ext4",
      0,
      NULL,
      0
   );

   MOUNT_CONFIG_INIT_OR_RETFAIL (
      g->newroot_usr_mount,
      GET_NEWROOT_PATH("/usr.squashfs"),
      GET_NEWROOT_PATH("/usr"),
      "squashfs",
      MS_RDONLY,
      NULL,
      MOUNT_CFG_NEED_LOOP
   );

   return 0;
}

static void _initramfs_globals_zap ( struct initramfs_globals_type* const g ) {

   g->root_ro_status          = -1;
   g->want_newroot_mount      = -1;
   g->want_newroot_usr_mount  = -1;
   g->rootfind_delay          = 0;
   g->doshell                 = INITRAMFS_DOSHELL_DISABLE;
   g->premount_status         = 0x0;
   g->have_network            = 0;

   g->want_debug              = 0;
   g->want_verbose            = 0;
   g->want_quiet              = 0;

   g->real_init               = NULL;
   g->shell                   = NULL;
   g->cmdline_argv            = NULL;
   g->failed_to_read_cmdline  = 0;

   g->kernel_release_str      = NULL;
   g->kernel_localver_str     = NULL;
   g->num_cpu                 = get_cpucount();
   g->memsize_m               = get_memsize_m();
   g->zram_autoswap_size_spec = NULL;

   g->load_modules            = NULL;
   g->rootfind_devices        = NULL;
   g->newroot_mount           = NULL;
   g->newroot_usr_mount       = NULL;

   g->logfile_stream          = NULL;
}

int _initramfs_globals_init ( struct initramfs_globals_type* const g ) {

   _initramfs_globals_zap ( g );

   if ( _initramfs_globals_init_data_structs ( g ) != 0 ) {
      _initramfs_globals_free ( g );
      return -1;
   }

   return 0;
}

void initramfs_globals_free_modprobe (void) {
   if ( initramfs_globals != NULL ) {
      _initramfs_globals_free_modprobe ( initramfs_globals );
   }
}

void initramfs_globals_free_real_init (void) {
   if ( initramfs_globals != NULL ) {
      _initramfs_globals_free_real_init ( initramfs_globals );
   }
}

void initramfs_globals_free_zram_autoswap (void) {
   if ( initramfs_globals != NULL ) {
      _initramfs_globals_free_zram_autoswap ( initramfs_globals );
   }
}

static int _initramfs_globals_open_logfile (
   struct initramfs_globals_type* const g,
   const char* const filepath, const char* const mode
) {
   _initramfs_globals_close_logfile_stream ( g );

   if ( filepath == NULL ) {
      return 0;
   } else {
      g->logfile_stream = fopen ( filepath, mode );
      return (g->logfile_stream == NULL) ? -1: 0;
   }
}

int initramfs_globals_open_logfile (void) {
   if ( initramfs_globals == NULL ) {
      return -2;
   } else {
      return _initramfs_globals_open_logfile (
         initramfs_globals, INITRAMFS_LOGFILE_PATH, "a"
      );
   }
}

void initramfs_globals_close_logfile (void) {
   if ( initramfs_globals != NULL ) {
      _initramfs_globals_close_logfile_stream ( initramfs_globals );
   }
}
