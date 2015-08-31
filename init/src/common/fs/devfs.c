/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h> /* PATH_MAX */

#include "devfs.h"
#include "stat.h"
#include "baseops.h"
#include "constants.h"
#include "proc_misc.h"
#include "fileio.h"
#include "../config.h"
#include "../mac.h"
#include "../message.h"
#include "../strutil/join.h"


struct _devfs_seed_buffer {
   char*   buf;
   size_t  devfs_dir_len;
   size_t  max_name_len;
};


int devfs_create_disk_node (
   const char* const sysfs_path,
   const char* const dev_path
) {
   unsigned maj;
   unsigned min;

   printf_debug (
      "(create_disk_node)", "create %s for %s", "\n", dev_path, sysfs_path
   );

   if (
      devfs_parse_maj_min_from_sysfs_dir ( sysfs_path, &maj, &min ) != 0
   ) {
      return -1;
   }

   return devfs_donod (
      dev_path, S_IFBLK, maj, min, RW_RW, 0, DEVFS_DISK_GID
   );
}


static int _devfs_check_is_device_node (
   mode_t devtype, const char* const path
);

int devfs_check_is_char_dev  ( const char* const path ) {
   return _devfs_check_is_device_node ( S_IFCHR, path );
}

int devfs_check_is_block_dev ( const char* const path ) {
   return _devfs_check_is_device_node ( S_IFBLK, path );
}

static void _devfs_parse_maj_min_substr_skip_whitespace (
   const char** const p_iter
) {
   while (1) {
      switch (**p_iter) {
         case ' ':
         case '\n':
         case '\r':
            (*p_iter)++;

         default:
            return;
      }
   }
}

static int _devfs_parse_maj_min_substr (
   const char*     str,
   unsigned* const pubuf,
   const char**    saveptr
) {
   const char* iter;

   if ( str != NULL ) {
      iter = str;
   } else if ( saveptr != NULL ) {
      iter = *saveptr;
   } else {
      return -1;
   }

   *saveptr = NULL;
   *pubuf   = 0;


   _devfs_parse_maj_min_substr_skip_whitespace ( &iter );
   if ( *iter == '\0' ) { return -1; }

   while (*iter != '\0') {
      switch (*iter) {
         case 0x30:
         case 0x31:
         case 0x32:
         case 0x33:
         case 0x34:
         case 0x35:
         case 0x36:
         case 0x37:
         case 0x38:
         case 0x39:
            *pubuf = ( 10 * (*pubuf) ) + (unsigned)(*iter) - 0x30;
            iter++;
            break;

         case ':':
            *saveptr = ++iter;
            return 0;

         default:
            _devfs_parse_maj_min_substr_skip_whitespace ( &iter );
            return (*iter == '\0') ? 0: -1;

      }
   }

   return 0;
}

int devfs_parse_maj_min_from_file (
   const char* const  filepath,
   unsigned* const    maj,
   unsigned* const    min
) {
   int   retcode;
   char* buf;

   buf = NULL;
   if ( read_sysfs_file ( filepath, &buf ) != 0 ) { return -1; }
   /* buf shouldn't be NULL if read_sysfs_file() succeeds */
   RETFAIL_IF_NULL ( buf );

   retcode = devfs_parse_maj_min ( buf, maj, min );
   x_free ( buf );

   return retcode;
}

int devfs_parse_maj_min_from_sysfs_dir (
   const char* const  dirpath,
   unsigned* const    maj,
   unsigned* const    min
) {
   int   retcode;
   char* fspath;

   RETFAIL_IF_NULL (
      fspath = join_str_pair ( dirpath, "/dev" )
   );

   retcode = devfs_parse_maj_min_from_file ( fspath, maj, min );

   x_free ( fspath );
   return retcode;
}

int devfs_parse_maj_min_from_sysfs_name (
   const char* const  name,
   unsigned* const    maj,
   unsigned* const    min
) {
   int   retcode;
   char* fspath;

   RETFAIL_IF_NULL (
      fspath = join_str_triple ( GET_SYSFS_PATH("/block/"), name, "/dev" )
   );

   retcode = devfs_parse_maj_min_from_file ( fspath, maj, min );

   x_free ( fspath );
   return retcode;
}


int devfs_parse_maj_min (
   const char* const  str,
   unsigned* const    maj,
   unsigned* const    min
) {
   unsigned buf;
   const char* saveptr;

   if ( maj == min ) { /* that won't work */ return -15; }

   *maj = 0;
   *min = 0;

   saveptr = NULL;
   if ( _devfs_parse_maj_min_substr ( str, &buf, &saveptr ) != 0 ) {
      return -1;
   }

   *maj = buf;

   if ( saveptr == NULL ) { return -2; }

   if ( _devfs_parse_maj_min_substr ( NULL, &buf, &saveptr ) != 0 ) {
      return -3;
   }

   *min = buf;

   if ( saveptr != NULL ) { return -4; }

   /*return ( (*maj < 256) && (*min < 256) ) ? 0 : -5;*/
   return 0;
}


int devfs_dochr (
   const char* const  path,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
) {
   return devfs_donod ( path, S_IFCHR, maj, min, mode, uid, gid );
}

int devfs_doblk (
   const char* const  path,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
) {
   return devfs_donod ( path, S_IFBLK, maj, min, mode, uid, gid );
}

int devfs_donod (
   const char* const  path,
   const mode_t       devtype,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
) {
   struct stat stat_info;
   dev_t dev;

   printf_debug (
      "(mknod)", "%s, %s %u,%u mode=%o uid=%u gid=%u", "\n",
      path,
      (
         devtype == S_IFCHR ? "chardev" : (
            devtype == S_IFBLK ? "blockdev" : "%%UNDEF%%"
         )
      ),
      maj, min, mode, uid, gid
   );

   dev = makedev ( maj, min );

   if ( stat ( path, &stat_info ) == 0 ) {
      /* node exists, check maj/min/devtype */
      if (
         (stat_info.st_dev == dev) && (stat_info.st_mode & devtype)
      ) {
         /* node is valid, apply chown/chmod if necessary */
         /* chown, chmod */

         if ( chmod_chown_stat ( path, &stat_info, mode, uid, gid ) != 0 ) {
            return 1;
         }

         return 0;
      }

      /* node is not valid, recreate it */
      if ( unlink ( path ) != 0 ) { return -1; }
   }

   if ( mknod ( path, devtype|(mode == 0 ? RW_USR : mode), dev ) != 0 ) {
      return -1;
   }

   return ( chown ( path, uid, gid ) == 0 ) ? 0: 1;
}

static int _devfs_check_is_device_node (
   mode_t devtype, const char* const path
) {
   struct stat stat_info;

   if ( stat ( path, &stat_info ) != 0 ) { return -1; }

   if ( (stat_info.st_mode & devtype) == 0 ) { return 1; }

   return 0;
}

static void _devfs_seed_init_buf_str (
   const struct _devfs_seed_buffer* const pbuf,
   const char* const devfs_dir
) {
   strncpy ( (pbuf->buf), devfs_dir, (pbuf->devfs_dir_len) );
   (pbuf->buf) [pbuf->devfs_dir_len]     = '/';
   (pbuf->buf) [(pbuf->devfs_dir_len)+1] = '\0';
}

static int _devfs_seed_fill_buffer (
   unsigned* const p_fail,
   const struct _devfs_seed_buffer* const pbuf,
   const char* const name
) {
   size_t i;

   /* "<=": also read terminating null char */
   for ( i = 0; i <= (pbuf->max_name_len); i++ ) {
      (pbuf->buf) [(pbuf->devfs_dir_len)+1+i] = name [i];
      if ( name[i] == '\0' ) { break; }
   }

   if ( name[i] == '\0' ) { return 0; }

   /* name too long */ \
   message_fprintf_color (
      MSG_ERRSTREAM,
      MSG_GET_COLOR(MAGENTA), "CODE ERROR", NULL,
      MSG_GET_COLOR(RED),
      "file %s, at line %d: buffer is too small:", " '%s'\n",
      __FILE__, __LINE__, name
   );

   *p_fail |= 4;
   return -1;
}


static void _devfs_seed_dosym (
   unsigned* const p_fail,
   const struct _devfs_seed_buffer* const pbuf,
   const unsigned failval,
   const char* const target,
   const char* const link_name
) {
   if ( _devfs_seed_fill_buffer ( p_fail, pbuf, link_name ) == 0 ) {
      if ( dosym ( target, pbuf->buf ) == 0 ) {
         MSG_PRINTF_DBG (
            "created symlink: %s -> %s\n", pbuf->buf, target
         );
      } else {
         MSG_PRINTF_DBG (
            "failed to create symlink: %s -> %s\n", pbuf->buf, target );

         *p_fail |= failval;
      }
   }
}

static void _devfs_seed_donod (
   unsigned* const p_fail,
   const struct _devfs_seed_buffer* const pbuf,
   const unsigned     failval,
   const mode_t       devtype,
   const char* const  name,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
) {
   if ( _devfs_seed_fill_buffer ( p_fail, pbuf, name ) != 0 ) { return; }

   switch ( devfs_donod ( pbuf->buf, devtype, maj, min, mode, uid, gid ) ) {
      case 0:
         MSG_PRINTF_DBG ( "created device node %s\n", pbuf->buf );
         break;

      case 1: \
         MSG_PRINTF_DBG (
            "created device node %s, but chmod/chown failed\n", pbuf->buf
         );
         break;

      default: \
         MSG_PRINTF_DBG ( "failed to create device node %s\n", pbuf->buf );
         *p_fail |= failval;
         break; /* nop */
   }
}

static void _devfs_seed_donod_dir_replace (
   unsigned* const p_fail,
   const struct _devfs_seed_buffer* const pbuf,
   const unsigned     failval,
   const mode_t       devtype,
   const char* const  dirname,
   const char* const  name,
   const unsigned     maj,
   const unsigned     min,
   const mode_t       mode,
   const uid_t        uid,
   const gid_t        gid
) {
   if ( _devfs_seed_fill_buffer ( p_fail, pbuf, dirname ) != 0 ) { return; }

   /* dodir unlinks the device node (if it exists and not a dir) */
   if ( dodir ( pbuf->buf ) == 0 ) {
      _devfs_seed_donod (
         p_fail, pbuf, failval, devtype,
         name, maj, min, mode, uid, gid
      );
   }
}

#ifdef DEVSEED__BUFCALL
#undef DEVSEED__BUFCALL
#endif

#define DEVSEED__DOSYM(failval,target,link_name) \
   DEVSEED__BUFCALL ( _devfs_seed_dosym, failval, target, link_name )

#define DEVSEED__DOCHR_RC(rc,...) \
   DEVSEED__BUFCALL ( _devfs_seed_donod, rc, S_IFCHR, __VA_ARGS__ )

#define DEVSEED__DOCHR(...)            DEVSEED__DOCHR_RC(1,__VA_ARGS__)
#define DEVSEED__DOCHR_NONFATAL(...)   DEVSEED__DOCHR_RC(0,__VA_ARGS__)

#define DEVSEED__DOCHR_DIR_REPL(rc,...) \
   DEVSEED__BUFCALL(_devfs_seed_donod_dir_replace,rc,S_IFCHR,__VA_ARGS__)



int devfs_seed ( const char* const devfs_dir ) {
#define DEVSEED__BUFCALL(F,...)   F(&fail,&sbuf,__VA_ARGS__)

   /* max name length is 15 ("pktcdvd/control") */
   static const size_t MAX_DEVNODE_NAME_LENGTH = 15;

   struct _devfs_seed_buffer sbuf = {
      .buf            = NULL, /* from stack */
      .devfs_dir_len  = strlen ( devfs_dir ),
      .max_name_len   = MAX_DEVNODE_NAME_LENGTH
   };

/*
   char      buf [sbuf.devfs_dir_len+1+MAX_DEVNODE_NAME_LENGTH+1];
*/
   char      buf [PATH_MAX];
   unsigned  fail;
   unsigned  min;

   fail     = 0;
   sbuf.buf = buf;

   if ( makedirs ( devfs_dir ) != 0 ) { return -8; }

   _devfs_seed_init_buf_str ( &sbuf, devfs_dir );

   DEVSEED__DOSYM ( 2, GET_PROC_PATH("/self/fd"),   "fd" );
   DEVSEED__DOSYM ( 2, GET_PROC_PATH("/self/fd/0"), "stdin" );
   DEVSEED__DOSYM ( 2, GET_PROC_PATH("/self/fd/1"), "stdout" );
   DEVSEED__DOSYM ( 2, GET_PROC_PATH("/self/fd/2"), "stderr" );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
   DEVSEED__DOCHR ( "console",   5,   1, RW_WO,    -1, -1 );
   DEVSEED__DOCHR ( "null",      1,   3, RW_RW_RW, -1, -1 );
   DEVSEED__DOCHR ( "ttyS0",     4,  64, RW_RW,    -1, DEVFS_TTY_GID );
   DEVSEED__DOCHR ( "tty1",      4,   1, RW_WO,    -1, DEVFS_TTY_GID );
   DEVSEED__DOCHR ( "tty",       5,   0, RW_RW_RW, -1, DEVFS_TTY_GID );
   DEVSEED__DOCHR ( "urandom",   1,   9, RW_RW_RW, -1, -1 );
   DEVSEED__DOCHR ( "random",    1,   8, RW_RW_RW, -1, -1 );
   DEVSEED__DOCHR ( "zero",      1,   5, RW_RW_RW, -1, -1 );

   DEVSEED__DOCHR_NONFATAL ( "kmsg", 1, 11, RW_RO_RO, -1, -1 );

   /* create /dev/pktcdvd/control */
   if ( proc_get_misc_chardev_minor ( "pktcdvd", &min ) == 0 ) {
      DEVSEED__DOCHR_DIR_REPL (
         0, "pktcdvd",
         "pktcdvd/control", 10, min, RW_RW, -1, DEVFS_CDROM_GID
      );
   }

   /* create /dev/mapper/control */
   if ( proc_get_misc_chardev_minor ( "device-mapper", &min ) == 0 ) {
      DEVSEED__DOCHR_DIR_REPL (
         0, "mapper",
         "mapper/control", 10, min, RW_USR, -1, -1
      );
   }

   /* create /dev/loop-control */
   if ( proc_get_misc_chardev_minor ( "loop-control", &min ) == 0 ) {
      DEVSEED__DOCHR ( "loop-control", 10, min, RW_RW, -1, DEVFS_DISK_GID );
   }

#pragma GCC diagnostic pop


   /* /dev/core */
   if ( access ( GET_PROC_PATH("/kcore"), F_OK ) == 0 ) {
      DEVSEED__DOSYM ( 0, GET_PROC_PATH("/kcore"), "core" );
   }

   sbuf.buf = NULL; /* not necessary */
   return -((int)fail);

#undef DEVSEED__BUFCALL
}


#undef DEVSEED__DOCHR_DIR_REPL
#undef DEVSEED__DOCHR_NONFATAL
#undef DEVSEED__DOCHR
#undef DEVSEED__DOCHR_RC
#undef DEVSEED__DOSYM
