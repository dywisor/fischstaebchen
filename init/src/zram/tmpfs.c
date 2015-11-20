/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>

#include "tmpfs.h"
#include "disk.h"
#include "data_types.h"
#include "config.h"
#include "zallocate.h"
#include "mkfs.h"
#include "mount.h"
#include "../common/mac.h"
#include "../common/message.h"
#include "../common/data_types/dynarray.h"
#include "../common/misc/sysinfo.h"
#include "../common/fs/constants.h"
#include "../common/fs/mount.h"
#include "../common/fs/mount_opts.h"
#include "../common/fs/mount_config.h"
#include "../common/strutil/compare.h"
#include "../common/strutil/convert.h"
#include "../common/strutil/join.h"

static const char* _zram_tmpfs_get_tmpfs_name (
   const struct zram_tmpfs_config* const p_zmount
) {
   return (p_zmount->zdisk.name == NULL) ? "tmpfs" : p_zmount->zdisk.name;
}

int zram_do_normal_tmpfs (
   const struct zram_tmpfs_config* const p_zmount
) {
   return stdmount (
      _zram_tmpfs_get_tmpfs_name ( p_zmount ),
      p_zmount->mp,
      "tmpfs",
      p_zmount->flags,
      p_zmount->tmpfs_opts_str
   );
}



int zram_do_zram_tmpfs (
   const struct zram_tmpfs_config* const p_zmount
) {
   int retcode;

   retcode = zram_disk_do_setup_and_mount (
      &(p_zmount->zdisk), p_zmount->mp, p_zmount->flags
   );

   if ( retcode == 0 ) {
      if ( (p_zmount->flags & MS_RDONLY) == 0 ) {
         if ( p_zmount->mode != 0 ) {
            if ( chmod ( p_zmount->mp, p_zmount->mode ) != 0 ) {
               retcode = -2;
            }
         }

         if ( (p_zmount->uid) || (p_zmount->gid) ) {
            if ( chown ( p_zmount->mp, p_zmount->uid, p_zmount->gid ) != 0 ) {
               retcode = -2;
            }
         }
      }
   }

   return retcode;
}

int zram_tmpfs (
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
) {
   int retcode;
   struct zram_tmpfs_config zmount;

   if ( mp == NULL ) {
      errno = EINVAL;
      return -1;
   }

   if (
      zram_tmpfs_config_parse (
         &zmount, name, mp, flags, opts, size_spec, sys_memsize_m,
         default_mode
      ) != 0
   ) {
      zram_tmpfs_config_free ( &zmount );
      return -1;
   }

   retcode = zram_do_zram_tmpfs ( &zmount );

   if ( retcode > 0 ) {
      print_debug ( "zram disk:", "falling back to tmpfs" );
      retcode = zram_do_normal_tmpfs ( &zmount );
   }

   zram_tmpfs_config_free ( &zmount );
   return 0;
}


void zram_tmpfs_config_free (
   struct zram_tmpfs_config* const p_zmount
) {
   zram_disk_config_free ( &(p_zmount->zdisk) );

   p_zmount->mp = NULL;

   x_free ( p_zmount->tmpfs_opts_str );
}

int zram_tmpfs_config_init (
   struct zram_tmpfs_config* const p_zmount,
   const char* const name,
   const char* const mp,
   const mode_t default_mode
) {
   if ( zram_disk_config_init ( &(p_zmount->zdisk), name ) != 0 ) {
      return -1;
   }

   p_zmount->mp               = mp;
   p_zmount->flags            = 0;
   p_zmount->tmpfs_opts_str   = NULL;
   p_zmount->mode             = default_mode;
   p_zmount->uid              = 0;
   p_zmount->gid              = 0;

   return 0;
}

struct zram_tmpfs_config* new_zram_tmpfs_config (
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
) {
   struct zram_tmpfs_config* p_zmount;

   RET_NULL_IFNOT_MALLOC_VAR ( p_zmount );

   if (
      zram_tmpfs_config_parse (
         p_zmount, name, mp, flags, opts, size_spec, sys_memsize_m,
         default_mode
      ) != 0
   ) {
      zram_tmpfs_config_free ( p_zmount );
      x_free ( p_zmount );
      return NULL;
   }

   return p_zmount;
}

static int _zram_tmpfs_config_parse_opts_arr (
   struct zram_tmpfs_config* const p_zmount,
   const struct dynarray* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m
);

int zram_tmpfs_config_parse (
   struct zram_tmpfs_config* const p_zmount,
   const char* const name,
   const char* const mp,
   unsigned long flags,
   const char* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m,
   const mode_t default_mode
) {
   int retcode;
   struct dynarray opts_arr;
   unsigned long extra_flags;

   if (
      zram_tmpfs_config_init (
         p_zmount, name, mp,
         ( default_mode == 0 ? SRWX_RWX_RWX : default_mode )
      ) != 0
   ) { return -1; }
   p_zmount->flags |= flags;

   if ( dynarray_init ( &opts_arr, 0 ) != 0 ) { return -1; }

   extra_flags = 0;
   if (
      ( opts != NULL ) && (
         parse_mount_opts_str_to_dynarray ( opts, &extra_flags, &opts_arr ) != 0
      )
   ) {
      retcode = -1;
   } else {
      p_zmount->flags |= extra_flags;

      retcode = _zram_tmpfs_config_parse_opts_arr (
         p_zmount, &opts_arr, size_spec,
         ( sys_memsize_m > 0 ? sys_memsize_m : get_memsize_m() )
      );

      if ( p_zmount->zdisk.size_m < 1 ) {
         print_error ( NULL, "failed to calculate zram tmpfs-disk size!" );
         retcode = -1;
      }
   }

   dynarray_free ( &opts_arr );
   return retcode;
}


static void _zram_tmpfs_config_parse_opts_arr_warn_ignoring_opt (
   const char* const opt
) {
   printf_warning (
      NULL,
      "failed to parse option '%s' - ignoring it.", "\n", opt
   );
}

static int _zram_tmpfs_config_parse_opts_arr (
   struct zram_tmpfs_config* const p_zmount,
   const struct dynarray* const opts,
   const char* const size_spec,
   const size_t sys_memsize_m
) {
   size_t k;
   const char* opt;
   const char* val;
   const char* any_size_spec;
   char* size_opt;
   long lbuf;
   struct dynarray tmpfs_opts_arr;

   if ( sys_memsize_m < 1 ) { print_warning ( NULL, "sys_memsize_m is 0!" ); }

   if ( dynarray_init ( &tmpfs_opts_arr, opts->len ) != 0 ) { return -1; }
   dynarray_set_data_readonly ( &tmpfs_opts_arr );

#define LX__ISPREFIX(s)  ( (val = str_startswith ( opt, s )) != NULL )

   any_size_spec = NULL;

   for ( k = 0; k < opts->len; k++ ) {
      opt = dynarray_get_str ( opts, k );

      if ( opt == NULL ) {
         /* ignore */

      } else if (
         LX__ISPREFIX ( "size=" ) || LX__ISPREFIX ( "x-size=" )
      ) {
         /* handle size arg */
         any_size_spec = val;

      } else if (
         LX__ISPREFIX ( "comp_algorithm=" ) || LX__ISPREFIX ( "comp=" )
      ) {
         printf_warning (
            NULL, "ignoring option %s: not implemented", "\n", opt
         );

      } else if LX__ISPREFIX ( "max_comp_streams=" ) {
         printf_warning (
            NULL, "ignoring option %s: not implemented", "\n", opt
         );

      } else {

         if LX__ISPREFIX ( "mode=" ) {
            /* mode */
            if ( str_to_long_base ( 8, val, &lbuf ) == 0 ) {
               p_zmount->mode = (mode_t)lbuf;
            } else {
               _zram_tmpfs_config_parse_opts_arr_warn_ignoring_opt ( opt );
            }

         } else if LX__ISPREFIX ( "uid=" ) {
            /* uid */
            if ( str_to_long_base ( 8, val, &lbuf ) == 0 ) {
               p_zmount->uid = (uid_t)lbuf;
            } else {
               _zram_tmpfs_config_parse_opts_arr_warn_ignoring_opt ( opt );
            }

         } else if LX__ISPREFIX ( "gid=" ) {
            /* gid */
            if ( str_to_long_base ( 8, val, &lbuf ) == 0 ) {
               p_zmount->gid = (gid_t)lbuf;
            } else {
               _zram_tmpfs_config_parse_opts_arr_warn_ignoring_opt ( opt );
            }

         } else if LX__ISPREFIX ( "nr_blocks=" ) {
            print_warning (
               NULL, "zram tmpfs mounts do not support nr_blocks option"
            );

         } else if LX__ISPREFIX ( "nr_inodes=" ) {
            print_warning (
               NULL, "zram tmpfs mounts do not support nr_blocks option"
            );

         } else if ( MSG_DBGSTREAM != NULL ) {
            if (
                  LX__ISPREFIX ( "context=" )
               || LX__ISPREFIX ( "fscontext=" )
               || LX__ISPREFIX ( "defcontext=" )
               || LX__ISPREFIX ( "rootcontext=" )
               || LX__ISPREFIX ( "mpol=" )
               /* owner, iversion, ... */
               || LX__ISPREFIX ( "x-" )
            ) {

            } else {
               printf_debug ( NULL, "unknown mount option: %s", "\n", opt );
            }
         }

         if ( dynarray_append_strnodup ( &tmpfs_opts_arr, opt ) != 0 ) {
            dynarray_free ( &tmpfs_opts_arr );
            return -1;
         }
      }

      opt = NULL;
   }
#undef LX__ISPREFIX

   size_opt = NULL;
   if ( size_spec != NULL ) { any_size_spec = size_spec; }

   switch (
      zram_disk_config_set_size (
         &(p_zmount->zdisk), any_size_spec, 0, sys_memsize_m
      )
   ) {
      case 0:
      case 1:
         size_opt = zram_disk_config_create_size_opt ( &(p_zmount->zdisk) );

         if ( size_opt != NULL ) {
            if (
               dynarray_append_strnodup ( &tmpfs_opts_arr, size_opt ) != 0
            ) {
               x_free ( size_opt );
               dynarray_free ( &tmpfs_opts_arr );
               return -1;
            }
         }

         break;
   }


   p_zmount->tmpfs_opts_str = join_str_dynarray ( ",", &tmpfs_opts_arr );
   dynarray_free ( &tmpfs_opts_arr );
   x_free ( size_opt );

   RETFAIL_IF_NULL ( p_zmount->tmpfs_opts_str );

   if ( *(p_zmount->tmpfs_opts_str) == '\0' ) {
      x_free ( p_zmount->tmpfs_opts_str );
   }

   return 0;
}
