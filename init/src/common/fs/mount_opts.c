/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "mount_opts.h"
#include "../config.h"
#include "../dynarray.h"
#include "../dynstr.h"
#include "../strutil/join.h"
#include "../strutil/compare.h"
#include "../strutil/format.h"
#include "../mac.h"
#include "../message.h"

char* get_mount_opts_str (
   const unsigned long flags,
   const char* const opts_in
) {
   /* dynarray + dyn-allocated str is super efficient */
   int retcode;
   struct dynarray darr;
   char* opts;

   if ( dynarray_init ( &darr, 0 ) != 0 ) { return NULL; }
   dynarray_set_data_readonly ( &darr );

#define _OAPPEND(bit,s)        (flags & bit) ? dynarray_append_strnodup(&darr,s) : 0
#define _OAPPEND_OR(bit,s,t)   dynarray_append_strnodup ( &darr, (flags & bit) ? s : t )
#define _CATCHFAIL(expr)       { retcode = expr; if ( retcode != 0 ) { break; }; }
#define OAPPEND(bit,s)         _CATCHFAIL(_OAPPEND(bit,s))
#define OAPPEND_OR(bit,s,t)    _CATCHFAIL(_OAPPEND_OR(bit,s,t))

   do {
      OAPPEND     ( MS_REMOUNT,     "remount" );
      OAPPEND     ( MS_BIND,        "bind" );
      OAPPEND     ( MS_MOVE,        "move");

      OAPPEND     ( MS_DIRSYNC,     "dirsync" );
      OAPPEND     ( MS_MANDLOCK,    "mand" );
      OAPPEND     ( MS_NOATIME,     "noatime" );
      OAPPEND     ( MS_NODEV,       "nodev" );
      OAPPEND     ( MS_NODIRATIME,  "nodiratime" );
      OAPPEND     ( MS_NOEXEC,      "noexec" );
      OAPPEND     ( MS_NOSUID,      "nosuid" );
      OAPPEND_OR  ( MS_RDONLY,      "ro", "rw" );
      OAPPEND     ( MS_RELATIME,    "relatime" );
      OAPPEND     ( MS_SILENT,      "silent" );
      OAPPEND     ( MS_STRICTATIME, "strictatime" );
      OAPPEND     ( MS_SYNCHRONOUS, "sync" );

      if ( opts_in != NULL ) {
         _CATCHFAIL ( dynarray_append_strnodup ( &darr, opts_in ) );
      }
   } while(0);

#undef OAPPEND_OR
#undef OAPPEND
#undef _CATCHFAIL
#undef _OAPPEND_OR
#undef _OAPPEND

   opts = join_str_dynarray ( ",", &darr );
   dynarray_free ( &darr );

   return opts;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

#define LX__SETFLAG_NEGATE(base_index,flag) \
   case base_index: \
      /* negate flag */ \
      flags &= ~flag; \
      break; \
   case base_index+1: \
      flags |= flag; \
      break;

#define LX__SETFLAG(base_index,flag) \
   case base_index: \
      flags |= flag; \
      break; \
   case base_index+1: \
      flags &= ~flag; \
      break;

int parse_mount_opts_str (
   const char* const opts_in,
   unsigned long* const flags_out,
   char** const opts_out_ptr
) {
   struct dynarray darr;

   *opts_out_ptr = NULL;

   if ( dynarray_init ( &darr, 0 ) != 0 ) { return -1; }

   if (
      parse_mount_opts_str_to_dynarray ( opts_in, flags_out, &darr ) == 0
   ) {
      *opts_out_ptr = join_str_dynarray ( ",", &darr );
   }

   dynarray_free ( &darr );

   return ( (*opts_out_ptr) != NULL ) ? 0: -1;
}


int parse_mount_opts_str_to_dynarray (
   const char* const opts_in,
   unsigned long* const flags_out,
   struct dynarray* const p_darr
) {
   char* saveptr;
   char* p_opt;
   char* opts_work_str;
   unsigned long flags;

   *flags_out = 0;

   if ( opts_in == NULL ) { return -1; }
   opts_work_str = strdup ( opts_in );
   if ( opts_work_str == NULL ) { return -1; }

   flags = 0;
   p_opt = strtok_r ( opts_work_str, ",", &saveptr );

   while ( p_opt != NULL ) {
      STREQ_SWITCH ( p_opt,
         "atime",
         "noatime",
         "auto",
         "noauto",
         "dev",
         "nodev",
         "diratime",
         "nodiratime",
         "exec",
         "noexec",
         "mand",
         "nomand",
         "relatime",
         "norelatime",
         "strictatime",
         "nostrictatime",
         "suid",
         "nosuid",
         "silent",
         "loud",
         "remount",
         "ro",
         "rw",
         "async",
         "sync",
         "bind",
         "dirsync",
         "loop",
         "_netdev",
         "defaults",
         "x-nopremount",
         "x-premount",

         NULL
      ) {
         case -1: /* did not match */
            /* append to opts str */
            if ( dynarray_append_strdup ( p_darr, p_opt ) != 0 ) {
               x_free ( opts_work_str );
               return -1;
            }
            break;


         LX__SETFLAG_NEGATE ( 0, MS_NOATIME )      /* atime, noatime */

         case 2: /* auto */
         case 3: /* noauto */
            /* don't care */
            break;

         LX__SETFLAG_NEGATE (  4, MS_NODEV )       /* dev, nodev */
         LX__SETFLAG_NEGATE (  6, MS_NODIRATIME )  /* diratime, nodiratime */
         LX__SETFLAG_NEGATE (  8, MS_NOEXEC )      /* exec, noexec */
         LX__SETFLAG        ( 10, MS_MANDLOCK )    /* mand, nomand */
         LX__SETFLAG        ( 12, MS_RELATIME )    /* relatime, norelatime */
         LX__SETFLAG        ( 14, MS_STRICTATIME ) /* strictatime, no... */
         LX__SETFLAG_NEGATE ( 16, MS_NOSUID )      /* suid, nosuid */
         LX__SETFLAG        ( 18, MS_SILENT )      /* silent, loud */

         case 20: /* remount */
            flags |= MS_REMOUNT;
            break;

         LX__SETFLAG        ( 21, MS_RDONLY )      /* ro, rw */
         LX__SETFLAG        ( 23, MS_SYNCHRONOUS ) /* async, sync */

         case 25: /* bind */
            flags |= MS_BIND;
            break;

         case 26: /* dirsync */
            flags |= MS_DIRSYNC;
            break;

         case 27: /* loop */
         case 28: /* _netdev */
         case 29: /* defaults */
         case 30: /* x-nopremount */
         case 31: /* x-premount */
            /* ignore */
            break;

         default:
            /* unhandled */
            printf_error (
               "BUG:", "did not parse mount option %s!", "\n", p_opt
            );

            /* append to opts str and keep going */
            if ( dynarray_append_strdup ( p_darr, p_opt ) != 0 ) {
               x_free ( opts_work_str );
               return -1;
            }
            break;
      }


      p_opt = strtok_r ( NULL, ",", &saveptr );
   }



   x_free ( opts_work_str );

   *flags_out = flags;
   return 0;
}
#undef LX__SETFLAG
#undef LX__SETFLAG_NEGATE

#pragma GCC diagnostic pop



static int _parse_size_spec (
   const long size_val,
   const char* const suffix,
   size_t system_memory_m,
   size_t* const size_out
);

int parse_tmpfs_size_spec (
   const char* const size_str, const size_t system_memory_m,
   size_t* const size_out
) {
   long ival;
   const char* suffix;

   *size_out = 0;

   if ( STR_IS_EMPTY ( size_str ) ) { return -1; }

   ival = strtol ( size_str, (char**)(&suffix), 0 );

   if ( suffix == size_str ) { return -1; }
   return _parse_size_spec ( ival, suffix, system_memory_m, size_out );
}

char* create_tmpfs_size_opt ( const size_t size_m ) {
   /* "size=" + %%size_m + '\0' */
#define BUFSIZE (5 + 39 + 1)
   char* buf;

   buf = malloc ( BUFSIZE );
   if ( buf != NULL ) {
      if ( str_format_check_fail ( buf, BUFSIZE, "size=%zum", size_m ) ) {
         x_free ( buf );
      }
   }

   return buf;
#undef BUFSIZE
}

static int _parse_size_spec (
   const long size_val,
   const char* const suffix,
   size_t system_memory_m,
   size_t* const size_out
) {
   if ( *suffix == '\0' ) {
      /* value in Bytes */
      *size_out = ( (size_t)size_val / (1024*1024) );

   } else if ( *(suffix+1) != '\0' ) {
      /* don't know any suffix longer than one char */
      return -1;

   } else {
      switch (*suffix) {
         case 'k':
            /* KiB */
            *size_out = ( (size_t)size_val / 1024 );
            break;

         case 'm':
            /* MiB */
            *size_out = (size_t)size_val;
            break;

         case 'g':
            /* GiB */
            *size_out = ( (size_t)size_val * 1024 );
            break;

         /* case 't': */

#if TMPFS_ENABLE_ADDITIONAL_SIZE_SUFFIXES
         /* COULDFIX:  (value * 1000^N) / (1024^2) */
         case 'K':
            /* kB */
            *size_out = ( (size_t)size_val / 1024 );
            break;

         case 'M':
            /* MB */
            *size_out = (size_t)size_val;
            break;

         case 'G':
            /* GB */
            *size_out = ( (size_t)size_val * 1024 );
            break;

         /* case 'T': */
#endif

         case '%':
            /* Percentage */
            if ( system_memory_m == 0 ) {
               return -1;
            }

            /* should not overflow */
            *size_out = ( (size_t)size_val * system_memory_m ) / 100;
            break;

         default:
            /* cannot parse this. */
            return -1;
      }
   }

   return 0;
}
