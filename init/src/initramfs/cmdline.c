/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../common/mac.h"
#include "../common/dynarray.h"
#include "../common/strutil/convert.h"
#include "../common/strutil/compare.h"
#include "../common/strutil/argsplit.h"
#include "../common/misc/sysinfo.h"
#include "../common/misc/kver.h"
#include "../common/fs/mount_opts.h"
#include "../common/fs/mount_config.h"

#include "../zram/autoswap.h"
#include "../zram/disk.h"

#include "devfs.h"
#include "globals.h"
#include "cmdline.h"
#include "config.h"


struct _cmdline_parse_data {
   struct initramfs_globals_type* const globals;

   /* data members, if any */
};

enum {
   CMDLINE_KEY_DONT_CARE,

   CMDLINE_KEY_MODPROBE,
   CMDLINE_KEY_INIT,
   CMDLINE_KEY_REAL_INIT,

   CMDLINE_KEY_NEWROOT_RO,
   CMDLINE_KEY_NEWROOT_RW,

   CMDLINE_KEY_NEWROOT_NOMOUNT,
   CMDLINE_KEY_NEWROOT_SOURCE,
   CMDLINE_KEY_NEWROOT_FSTYPE,
   CMDLINE_KEY_NEWROOT_FLAGS,
   CMDLINE_KEY_STAGEDIVE,

   CMDLINE_KEY_ROOTFIND,

   CMDLINE_KEY_USR_SFS_NOMOUNT,
   CMDLINE_KEY_USR_SFS,

   CMDLINE_KEY_ZRAM_SWAP,
   CMDLINE_KEY_ZRAM_DISK,

   CMDLINE_KEY_SHELL,

   CMDLINE_KEY_LIRAM,
   CMDLINE_KEY_LIRAM_STH,

   CMDLINE_KEY__LAST
};


int initramfs_process_cmdline (void) {
   return _initramfs_process_cmdline ( initramfs_globals );
}

static int _initramfs_cmdline_process_arg (
   const struct argsplit_data* const arg,
   int* const flow_ctrl, void* const p_data
);

static int _initramfs_postprocess_cmdline (
   struct _cmdline_parse_data* const pshare
);

int _initramfs_process_cmdline ( struct initramfs_globals_type* const g ) {
   int retcode;
   struct _cmdline_parse_data shared_parse_data = {
      .globals = g
   };

   /* init data */
   g->num_cpu   = get_cpucount();
   g->memsize_m = get_memsize_m();

   if ( g->kernel_release_str == NULL ) {
      g->kernel_release_str  = get_kernel_release();
      g->kernel_localver_str = get_localversion ( g->kernel_release_str );
   }

   retcode = argsplit_dynarray_do (
      g->cmdline_argv, "=", ",",
      _initramfs_cmdline_process_arg, &shared_parse_data
   );

   if ( retcode == 0 ) {
      retcode = _initramfs_postprocess_cmdline ( &shared_parse_data );
   }

   /* cleanup actions */

   return retcode;
}

static int _initramfs_postprocess_cmdline (
   struct _cmdline_parse_data* const pshare
) {

   switch (pshare->globals->root_ro_status) {
      case 0:
         pshare->globals->newroot_mount->flags &= (unsigned long)~MS_RDONLY;
         break;

      default:
         pshare->globals->newroot_mount->flags |= MS_RDONLY;
         break;
   }

   if ( mount_config_is_enabled ( pshare->globals->newroot_mount ) == 0 ) {
      mount_config_autoset ( pshare->globals->newroot_mount );
   }

   if ( mount_config_is_enabled ( pshare->globals->newroot_usr_mount ) == 0 ) {
      mount_config_autoset ( pshare->globals->newroot_usr_mount );
   }


   return 0;
}



static int _initramfs_cmdline_get_int_key ( const char* const key );

static int _initramfs_cmdline_process_arg__shell (
   struct initramfs_globals_type* const g,
   const struct argsplit_data* const arg

) {
   if ( arg->end_of_arg < 0 ) {
      if ( arg->nargs < 2 ) {
         g->doshell = INITRAMFS_DOSHELL_LOOP;
      }

   } else if ( arg->end_of_arg == 0 ) {
      if ( STR_IS_NOT_EMPTY ( arg->value ) ) {
         STREQ_SWITCH ( arg->value,
            "never", "fail", "error", "once", "pre", "preswitch", "always"
          ) {
            case 0:  /* never */
               g->doshell = INITRAMFS_DOSHELL_DISABLE;
               break;

            case 1:  /* fail */
            case 2:  /* error */
               g->doshell = INITRAMFS_DOSHELL_ONERROR;
               break;

            case 3:  /* once */
               g->doshell = INITRAMFS_DOSHELL_ONCE;
               break;

            case 4:  /* pre */
            case 5:  /* preswitch */
               g->doshell = INITRAMFS_DOSHELL_ONCE_PRESWITCH;
               break;

            case 6:  /* always */
               g->doshell = INITRAMFS_DOSHELL_LOOP;
               break;

            default:
               initramfs_warn (
                  "unknown value for %s cmdline arg: %s", "\n",
                  arg->key, arg->value
               );
               break; /* nop */
         }
      }
   }

   return 0;
}

static int _initramfs_cmdline_process_arg__rootfsflags (
   struct initramfs_globals_type* const g, int* const flow_ctrl,
   const struct argsplit_data* const arg
) {
   const char* valstr;
   unsigned long flags;
   char* opts;


   valstr = strchr ( arg->str, '=' ); /* ugly */
   if (valstr == NULL) { return -1; }

   valstr++;

   if ( *valstr == '\0' ) {
      /* clear flags / opts */
      mount_config_free_opts ( g->newroot_mount );
      g->newroot_mount->flags = 0;

   } else {
      if ( parse_mount_opts_str ( valstr, &flags, &opts ) != 0 ) {
         return -1;
      }

      /* set flags / opts */

      /* refcopy opts, no need to strdup it */
      mount_config_free_opts ( g->newroot_mount );
      g->newroot_mount->opts = opts;
      opts = NULL;

      g->newroot_mount->flags = flags;
   }

   *flow_ctrl = ARGSPLIT_NEXT_ARG;
   return 0;
}

static int _initramfs_cmdline_process_arg__zram_swap (
   struct initramfs_globals_type* const g, int* const UNUSED(flow_ctrl),
   const struct argsplit_data* const arg
) {
   if ( arg->end_of_arg >= 0 ) { return 0; }

   if ( arg->nargs > 2 ) {
      initramfs_warn (
         "%s arg accepts at most one option, ignoring remainder.", "\n",
         arg->key
      );
   }

   _initramfs_globals_free_zram_autoswap(g);

   if (
      ( arg->nargs == 1 ) || STR_IS_EMPTY ( (arg->argv)[1] )
   ) {
      g->zram_autoswap_size_spec = strdup (
         zram_autoswap_guess_fractional ( g->memsize_m )
      );

   } else {
      g->zram_autoswap_size_spec = strdup ( (arg->argv)[1] );
   }

   return 0;
}


static int _initramfs_cmdline_process_arg__zram_disk (
   const struct initramfs_globals_type* const g, int* const flow_ctrl,
   const struct argsplit_data* const arg
) {
   size_t k;
   const char* opt;
   const char* val;

   const char* p_name;
   const char* p_size_spec;


   if ( arg->end_of_arg != 0 ) { return 0; }

   p_name      = NULL;
   p_size_spec = NULL;

   for ( k = 0; k < arg->nargs; k++ ) {
      opt = (arg->argv) [k];

      if ( STR_IS_EMPTY ( opt ) ) {
         /* ignore */

      } else if ( (k == 0) && (strchr ( opt, '=' ) == NULL) ) {
         /* opt's our fsname */
         p_name = opt;

      } else if ( (val = str_startswith ( opt, "size=" )) != NULL ) {
         /* val's our size */
         p_size_spec = val;

      } else if ( (val = str_startswith ( opt, "name=" )) != NULL ) {
         /* val's our fsname */
         p_name = val;

      } else if ( strcmp ( opt, "lz4" ) == 0 ) {
         /* not implemented */
         initramfs_debug ( "zdisk option not implemented: %s", "\n", opt );

      } else if ( strcmp ( opt, "lzo" ) == 0 ) {
         /* not implemented */
         initramfs_debug ( "zdisk option not implemented: %s", "\n", opt );

      } else if (
            ( (val = str_startswith ( opt, "comp=" )) != NULL )
         || ( (val = str_startswith ( opt, "alg=" ))  != NULL )
      ) {
         /* recognized, but not implemented */
         initramfs_debug ( "zdisk option not implemented: %s", "\n", opt );

      } else if (
         (val = str_startswith ( opt, "streams=" )) != NULL
      ) {
         /* recognized, but not implemented */
         initramfs_debug ( "zdisk option not implemented: %s", "\n", opt );

      } else {
         /* unknown option */
         initramfs_warn ( "unknown zdisk option %s", "\n", opt );
      }

      opt = NULL;
   }

   if ( STR_IS_EMPTY ( p_name )      ) { p_name      = NULL; }
   if ( STR_IS_EMPTY ( p_size_spec ) ) { p_size_spec = NULL; }

   *flow_ctrl = ARGSPLIT_NEXT_ARG;
   return zram_disk_simple ( p_name, NULL, p_size_spec, g->memsize_m );
}



static int _initramfs_cmdline_process_arg__rootfind (
   struct initramfs_globals_type* const g,
   const struct argsplit_data* const arg
) {
   static const uint DEFAULT_ROOTFIND_DELAY = 50;
   long lbuf;

   switch ( arg->end_of_arg ) {
      case -1:
         /* set default delay if not already configured */
         if ( g->rootfind_delay == 0 ) {
            g->rootfind_delay = DEFAULT_ROOTFIND_DELAY;
         }
         break;

      case 0:
         if ( STR_IS_NOT_EMPTY ( arg->value ) ) {
            if (
                  ( str_to_long ( arg->value, &lbuf ) != 0 )
               || (lbuf < 0)
               || (lbuf > 10000) /* that's over 16 minutes! */
            ) {
               g->rootfind_delay = DEFAULT_ROOTFIND_DELAY;
            } else {
               g->rootfind_delay = (unsigned)lbuf;
            }
         }
         break;
   }

   return 0;
}

static int _initramfs_cmdline_process_arg__sfsmount_any (
   struct mount_config* const p_mount,
   const struct argsplit_data* const arg
) {
   /* const char* val; */

   switch ( arg->end_of_arg ) {
      case -1:
         mount_config_enable ( p_mount, MOUNT_CFG_IS_ENABLED );
         break;

      case 0:
         if ( STR_IS_EMPTY ( arg->value ) ) {

         } else if ( strcmp ( arg->value, "rw" ) == 0 ) {
            p_mount->flags &= (unsigned long)~MS_RDONLY;

         } else if ( strcmp ( arg->value, "ro" ) == 0 ) {
            p_mount->flags |= MS_RDONLY;

         } else if ( *(arg->value) == '/' ) {
            mount_config_set_source ( p_mount, arg->value );

         /* ignore unknown options */
/*
         } else if ( strcmp ( arg->value, "mem" ) == 0 ) {
            // backing device is tmpfs

         } else if (
            ( (val = str_startswith ( arg->value, "mem=" )) != NULL )
            || ( (val = str_startswith ( arg->value, "size=" )) != NULL )
         ) {
            // backing device is tmpfs, with the specified size


         } else if (
            (val = str_startswith ( arg->value, "disk=" )) != NULL
         ) {
            // backing device is a blockdev


         } else if (
            (val = str_startswith ( arg->value, "fstype=" )) != NULL
         ) {
            // fstype of the backing device, may also be "zram"


         } else {
            initramfs_warn (
               "unknown sfs-mount option: %s", "\n", arg->value
            );
*/
         }
         break;
   }

   return 0;
}

static int _initramfs_cmdline_process_arg__squashed_usr (
   struct initramfs_globals_type* const g,
   const struct argsplit_data* const arg
) {
   return _initramfs_cmdline_process_arg__sfsmount_any (
      g->newroot_usr_mount, arg
   );
}

static int _initramfs_cmdline_process_arg (
   const struct argsplit_data* const arg,
   int* const flow_ctrl, void* const p_data
) {
   int retcode;
   struct _cmdline_parse_data* const pshare = p_data;

   if ( pshare == NULL ) { return -1; }

   retcode = 0;

   switch ( _initramfs_cmdline_get_int_key(arg->key) ) {
      case CMDLINE_KEY_DONT_CARE:
         break;

      case CMDLINE_KEY_MODPROBE:
         if ( (arg->end_of_arg == 0) && STR_IS_NOT_EMPTY ( arg->value ) ) {
            retcode = dynarray_append_strdup (
               pshare->globals->load_modules, arg->value
            );
         }
         break;

      case CMDLINE_KEY_INIT:
      case CMDLINE_KEY_REAL_INIT:
         if ( (arg->end_of_arg == 0) && (arg->value != NULL) ) {
            _initramfs_globals_free_real_init ( pshare->globals );

            pshare->globals->real_init = strdup ( arg->value );
            retcode = (pshare->globals->real_init == NULL) ? -1 : 0;
         }
         break;

      case CMDLINE_KEY_NEWROOT_RO:
         if ( arg->end_of_arg < 0 ) {
            pshare->globals->root_ro_status = 1;
            *flow_ctrl = ARGSPLIT_NEXT_ARG;
         }
         break;

      case CMDLINE_KEY_NEWROOT_RW:
         if ( arg->end_of_arg < 0 ) {
            pshare->globals->root_ro_status = 0;
            *flow_ctrl = ARGSPLIT_NEXT_ARG;
         }
         break;

      case CMDLINE_KEY_NEWROOT_NOMOUNT:
         pshare->globals->want_newroot_mount = 0;
         break;

      case CMDLINE_KEY_NEWROOT_SOURCE:
         if ( arg->end_of_arg == 0 ) {
            if ( STR_IS_NOT_EMPTY ( arg->value ) ) {
               retcode = mount_config_set_source (
                  pshare->globals->newroot_mount, arg->value
               );
               mount_config_enable (
                  pshare->globals->newroot_mount, MOUNT_CFG_IS_ENABLED
               );
               *flow_ctrl = ARGSPLIT_NEXT_ARG;
            }
         }
         break;

      case CMDLINE_KEY_NEWROOT_FSTYPE:
         if ( arg->end_of_arg == 0 ) {
            if ( STR_IS_NOT_EMPTY ( arg->value ) ) {
               retcode = mount_config_set_fstype (
                  pshare->globals->newroot_mount, arg->value
               );
            }

            *flow_ctrl = ARGSPLIT_NEXT_ARG;
         }
         break;

      case CMDLINE_KEY_NEWROOT_FLAGS:
         retcode = _initramfs_cmdline_process_arg__rootfsflags (
            pshare->globals, flow_ctrl, arg
         );
         break;

      case CMDLINE_KEY_ROOTFIND:
         retcode = _initramfs_cmdline_process_arg__rootfind (
            pshare->globals, arg
         );
         break;

      case CMDLINE_KEY_STAGEDIVE:
         retcode = mount_config_set_source (
            pshare->globals->newroot_mount, "zroot"
         );
         if ( retcode == 0 ) {
            retcode = mount_config_set_fstype (
               pshare->globals->newroot_mount, "zdisk"
            );

            if ( retcode == 0 ) {
               pshare->globals->root_ro_status = 0;
               mount_config_free_opts ( pshare->globals->newroot_mount );
               mount_config_enable (
                  pshare->globals->newroot_mount, MOUNT_CFG_IS_ENABLED
               );
               pshare->globals->want_newroot_mount = 1;
            }
         }

         *flow_ctrl = ARGSPLIT_NEXT_ARG;
         break;

      case CMDLINE_KEY_USR_SFS:
         /*
          * pshare->globals->want_newroot_usr_mount can only be disabled,
          * don't bother parsing squashed_usr if it has been disabled
          * */
         if ( pshare->globals->want_newroot_usr_mount ) {
            retcode = _initramfs_cmdline_process_arg__squashed_usr (
               pshare->globals, arg
            );
         }
         break;

      case CMDLINE_KEY_USR_SFS_NOMOUNT:
         pshare->globals->want_newroot_usr_mount = 0;
         break;

      case CMDLINE_KEY_ZRAM_SWAP:
         retcode = _initramfs_cmdline_process_arg__zram_swap (
            pshare->globals, flow_ctrl, arg
         );
         break;

      case CMDLINE_KEY_ZRAM_DISK:
         retcode = _initramfs_cmdline_process_arg__zram_disk (
            pshare->globals, flow_ctrl, arg
         );
         break;

      case CMDLINE_KEY_SHELL:
         retcode = _initramfs_cmdline_process_arg__shell (
            pshare->globals, arg
         );
         break;

      case CMDLINE_KEY_LIRAM:
      case CMDLINE_KEY_LIRAM_STH:
         /* while liram options are not parsed here [yet?],
          * disable want_newroot_mount as it conflicts with liram
          * */
          /* FIXME: newroot_mount supports [z]tmpfs now, no need to disable this. */
          pshare->globals->want_newroot_mount = 0;
          break;

      default:
         initramfs_err (
            "did not handle cmdline arg with key '%s'", "\n", arg->key
         );
         break; /* nop */
   }

   return retcode;
}


static int _initramfs_cmdline_get_int_key ( const char* const key ) {
   if ( STR_IS_EMPTY ( key ) ) { return CMDLINE_KEY_DONT_CARE; }

   STREQ_SWITCH ( key,

      "modprobe",
      "load_modules",

      "init",
      "real_init",

      "ro",
      "rw",

      "norootmount",
      "root",
      "rootfstype",
      "rootfsflags",
      "rootfind",

      "squashed_usr",

      "zram_swap",
      "zram_disk",
      "zdisk",

      "xshell",

      "liram",
      "stagedive",

      "nousrmount"
   ) {
      case -1:
         if ( str_startswith ( key, "liram_" ) != NULL ) {
            return CMDLINE_KEY_LIRAM_STH;
         }

         return CMDLINE_KEY_DONT_CARE;

      case 0:  /* modprobe */
      case 1:  /* load_modules */
         return CMDLINE_KEY_MODPROBE;

      case 2:  /* init */
         return CMDLINE_KEY_INIT;

      case 3:  /* real_init */
         return CMDLINE_KEY_REAL_INIT;

      case 4:  /* ro */
         return CMDLINE_KEY_NEWROOT_RO;

      case 5:  /* rw */
         return CMDLINE_KEY_NEWROOT_RW;

      case 6:  /* norootmount */
         return CMDLINE_KEY_NEWROOT_NOMOUNT;

      case 7:  /* root */
         return CMDLINE_KEY_NEWROOT_SOURCE;

      case 8:  /* rootfstype */
         return CMDLINE_KEY_NEWROOT_FSTYPE;

      case 9:  /* rootfsflags */
         return CMDLINE_KEY_NEWROOT_FLAGS;

      case 10: /* rootfind */
         return CMDLINE_KEY_ROOTFIND;

      case 11: /* squashed_usr */
         return CMDLINE_KEY_USR_SFS;

      case 12: /* zram_swap */
         return CMDLINE_KEY_ZRAM_SWAP;

      case 13: /* zram_disk */
      case 14: /* zdisk */
         return CMDLINE_KEY_ZRAM_DISK;

      case 15: /* xshell */
         return CMDLINE_KEY_SHELL;

      case 16: /* liram */
         return CMDLINE_KEY_LIRAM;

      case 17: /* stagedive */
         return CMDLINE_KEY_STAGEDIVE;

      case 18: /* nousrmount */
         return CMDLINE_KEY_USR_SFS_NOMOUNT;

      default:
         initramfs_err (
            "did not deref cmdline arg with key '%s'", "\n", key
         );
         return CMDLINE_KEY_DONT_CARE;
   }
}
