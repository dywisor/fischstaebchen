/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mntent.h>
#include <string.h>
#include <unistd.h>


#include "umount.h"
#include "../dynarray.h"
#include "../mac.h"
#include "../message.h"
#include "../strutil/compare.h"




struct umount_info {
   int   depth;
   char* mp;
};

int do_umount ( const char* const mp ) {

   printf_message ( NULL, "Unmounting %s", "\n", mp );

   errno = 0;
   if ( umount ( mp ) == 0 ) { return 0; }

   switch (errno) {
      case EINVAL:
      case ENOENT:
         return 1;

      case EBUSY:
         printf_warning ( NULL, "Lazy-unmounting %s", "\n", mp );

         errno = 0;
         if ( umount2 ( mp, MNT_DETACH ) == 0 ) { return 2; }

         switch (errno) {
            case EINVAL:
            case ENOENT:
               return 1;
         }
         break;

   }

   printf_error ( NULL, "Failed to unmount %s", "\n", mp );
   return -1;
}



int umount_all_in ( const char* const root ) {
   return do_umount_recursive ( root, 1 );
}

int umount_recursive ( const char* const root ) {
   return do_umount_recursive ( root, 0 );
}


static int _read_mountpoint_to_dynarray (
   struct dynarray* const p_darr,
   const char* const root, const char* const mtab_file,
   const int mindepth
);

static void _umountsort_dynarray ( const struct dynarray* const p_darr );

static int _do_umount_dynarray (
   const struct dynarray* const p_darr
);

static void _do_free_umount_dynarray ( struct dynarray* const p_darr );


int do_umount_recursive ( const char* const root, const int mindepth ) {
   int retcode;
   struct dynarray darr;

   if ( dynarray_init ( &darr, 8 ) != 0 ) { return -1; }

   retcode = _read_mountpoint_to_dynarray (
      &darr, root, "/proc/mounts", mindepth
   );

   if ( retcode >= 0 ) {
      _umountsort_dynarray ( &darr );
      retcode = _do_umount_dynarray ( &darr );
   }

   _do_free_umount_dynarray ( &darr );
   return retcode;
}



static int _get_mount_depth ( const char* const mp ) {
   int depth;
   const char* s;

   s = mp;
   if ( s == NULL) { return -1; }

   depth = 0;
   while ( *s != '\0' ) {
      switch ( *s ) {
         case '/':
            depth++;
            do { s++; } while ( *s == '/' );
            break;

         default:
            s++;
            break; /* nop */
      }
   }

   return depth;
}


static struct umount_info* _create_umount_info (
   const char* const mp, const int depth
) {
   struct umount_info* u;

   RET_NULL_IFNOT_MALLOC_VAR ( u );

   u->mp = strdup ( mp );
   if ( u->mp == NULL ) { x_free ( u ); return NULL; }

   u->depth = depth;

   return u;
}

static int _add_mountpoint_to_dynarray (
   struct dynarray* const p_darr,
   const char* const root,
   const int mindepth,
   const char* const mp
) {
   struct umount_info* p_uinf;
   const char* rem;
   int depth;

   RETFAIL_IF_NULL (
      rem = (root == NULL) ? mp : str_startswith ( mp, root )
   );

   switch ( *rem ) {
      case '\0':
      case '/':
         depth = _get_mount_depth ( mp ); /* FIXME: or depth of rem? */

         if ( depth >= mindepth ) {
            RETFAIL_IF_NULL (
               p_uinf = _create_umount_info ( mp, depth )
            );

            if ( dynarray_append ( p_darr, p_uinf ) != 0 ) {
               /* might memleak, but we're already in OOM code */
               x_free ( p_uinf );
               return -1;
            }
         }

         return 0;

   }

   return 1;
}

static inline int _is_empty_or_rootfs ( const char* const s ) {
   if ( STR_IS_EMPTY(s) ) { return 0; }

   if ( ( *(s+1) == '\0' ) && ( *s == '/' ) ) { return 0; }

   return 1;
}

static inline const char* _deref_root_str ( const char* const root ) {
   return ( _is_empty_or_rootfs ( root ) == 0 ) ? NULL : root;
}

static int _read_mountpoint_to_dynarray (
   struct dynarray* const p_darr,
   const char* const root, const char* const mtab_file,
   const int mindepth
) {
   FILE* fstream;
   struct mntent* entry;
   const char* my_root;

   fstream = setmntent ( mtab_file, "r" );
   if ( fstream == NULL ) { return -1; }

   my_root = _deref_root_str ( root );

   while ( ( entry = getmntent ( fstream ) ) != NULL ) {
      if ( STR_IS_NOT_EMPTY ( entry->mnt_dir ) ) {

         if ( strcmp ( entry->mnt_type, "autofs" ) != 0 ) {

            if (
               _add_mountpoint_to_dynarray (
                  p_darr, my_root, mindepth, entry->mnt_dir
               ) < 0
            ) {
               fclose ( fstream );
               return -1;
            }
         }
      }
   }

   fclose ( fstream );
   return 0;
}

#define dynarray_get_uinfo(d,X)   ( (struct umount_info*) (dynarray_get(d,X)) )
#define dynarray_get_udepth(d,X)  (dynarray_get_uinfo(d,X))->depth

static void _do_free_umount_dynarray ( struct dynarray* const p_darr ) {
   size_t k;
   struct umount_info* u;

   for ( k = 0; k < (p_darr->len); k++ ) {
      u = dynarray_get_uinfo ( p_darr, k );

      if ( u != NULL ) {
         x_free ( u->mp );

         /* not necessary, dynarray_free() can handle this, too */
         x_free ( u );
         (p_darr->arr)[k] = NULL;
      }
   }

   dynarray_free ( p_darr );
}

static void _umountsort_dynarray ( const struct dynarray* const p_darr ) {
   /* insertion sort. */
   size_t k;
   size_t j;
   struct umount_info* uinsert;

   for ( k = 1; k < (p_darr->len); k++ ) {
      uinsert = dynarray_get_uinfo ( p_darr, k );

      j = k;

      while (
         (j > 0) && ( dynarray_get_udepth ( p_darr, j-1 ) < (uinsert->depth) )
      ) {
         (p_darr->arr)[j] = dynarray_get ( p_darr, j-1 );
         j--;
      }

      (p_darr->arr)[j] = uinsert;
      uinsert = NULL;
   }
}

static int _do_umount_dynarray ( const struct dynarray* const p_darr ) {
   int retcode;
   size_t k;
   struct umount_info* uinfo;

   retcode = 0;

   for ( k = 0; k < (p_darr->len); k++ ) {
      uinfo = dynarray_get_uinfo ( p_darr, k );

      if ( uinfo->mp != NULL ) {

         switch ( do_umount ( uinfo->mp ) ) {
            case 0:
            case 1:
            case 2:
               break;

            default:
               retcode = -1;
         }
      }
   }

   return retcode;
}

#undef dynarray_get_udepth
#undef dynarray_get_uinfo
