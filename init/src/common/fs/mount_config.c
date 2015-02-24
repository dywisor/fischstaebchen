/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mount_config.h"
#include "mount_opts.h"
#include "mount.h"
#include "findfs.h"
#include "loopdev.h"
#include "constants.h"
#include "../strutil/compare.h"
#include "../message.h"
#include "../mac.h"

static int mount_config_check_valid_verbose (
   const struct mount_config* const p_mount
);



const char* domount_strerror ( const int mnterr ) {
   switch (mnterr >= 0 ? mnterr : -mnterr) {
      case DOMOUNT_SUCCESS:
         return "success";

      case DOMOUNT_ERR_UNKNOWN:
         return "unknown error";

      case DOMOUNT_ERR_INVALID:
         return "invalid config";

      case DOMOUNT_ERR_FINDFS:
         return "failed to locate device (findfs)";

      case DOMOUNT_ERR_LOSETUP:
         return "failed to set up loop device(s)";

      case DOMOUNT_ERR_MOUNT_INTERN:
         return "internal mount function failed";

      case DOMOUNT_ERR_MOUNT_INTERN_BIND:
         return "internal bind-mount function failed";

      case DOMOUNT_ERR_MOUNT_EXTERN:
         return "external mount binary failed";

      default:
         return "undefined error";
   }
}


#define DECLARE_MOUNT_CONFIG_FREE(A) \
   void (mount_config_free_ ## A) ( \
      struct mount_config* p_mount \
   ) { \
      x_free ( p_mount->A ); \
   }

#define DECLARE_MOUNT_CONFIG_STR_SETTER(A) \
   int (mount_config_set_ ## A) ( \
      struct mount_config* const p_mount, \
      const char* const A \
   ) { \
      (mount_config_free_ ## A) ( p_mount ); \
      if ( A == NULL ) { return 0; } \
      p_mount->A = strdup(A); \
      return (p_mount->A != NULL) ? 0 : -1; \
   }

#define DECLARE_MOUNT_CONFIG_STR_MEMBER_FUNCS(A) \
   DECLARE_MOUNT_CONFIG_FREE(A) \
   DECLARE_MOUNT_CONFIG_STR_SETTER(A)

/* str members */
DECLARE_MOUNT_CONFIG_STR_MEMBER_FUNCS ( source )
DECLARE_MOUNT_CONFIG_STR_MEMBER_FUNCS ( target )
DECLARE_MOUNT_CONFIG_STR_MEMBER_FUNCS ( fstype )
DECLARE_MOUNT_CONFIG_STR_MEMBER_FUNCS ( opts )

#undef DECLARE_MOUNT_STR_CONFIG_MEMBER_FUNCS
#undef DECLARE_MOUNT_CONFIG_STR_SETTER
#undef DECLARE_MOUNT_CONFIG_FREE


int mount_config_init (
   struct mount_config* const p_mount,
   const char* const  source,
   const char* const  target,
   const char* const  fstype,
   unsigned long      flags,
   const char* const  opts,
   unsigned long      cfg
) {
#define SCOPY(A)  \
   do { \
      if ( A != NULL ) { \
         scopy = strdup(A); \
         if ( scopy == NULL ) { \
            mount_config_free_members(p_mount); return -1; \
         } else { \
            p_mount->A = scopy; scopy = NULL; \
         } \
      } \
   } while(0)

   char *scopy;

   if ( p_mount == NULL ) { return -1; }

   p_mount->cfg        = cfg;
   p_mount->source     = NULL;
   p_mount->target     = NULL;
   p_mount->fstype     = NULL;
   p_mount->flags      = flags;
   p_mount->opts       = NULL;

   SCOPY ( source );
   SCOPY ( target );
   SCOPY ( fstype );
   SCOPY ( opts );

   return 0;
#undef SCOPY
}


int mount_config_ptr_init (
   struct mount_config** const pp_mount,
   const char* const  source,
   const char* const  target,
   const char* const  fstype,
   unsigned long      flags,
   const char* const  opts,
   unsigned long      cfg
) {
   if ( pp_mount == NULL ) { return -1; }
   if ( *pp_mount != NULL ) { return 1; }

   *pp_mount = malloc (sizeof **pp_mount);

   if ( *pp_mount == NULL ) {
      return -1;

   } else if (
      mount_config_init (
         *pp_mount,
         source, target, fstype, flags, opts, cfg
      ) != 0
   ) {
      x_free ( *pp_mount );
      return -2;
   }

   return 0;
}

void mount_config_free_members (
   struct mount_config* const p_mount
) {
   mount_config_free_source ( p_mount );
   mount_config_free_target ( p_mount );
   mount_config_free_fstype ( p_mount );
   mount_config_free_opts   ( p_mount );

   p_mount->cfg   = 0;
   p_mount->flags = 0;
}

void mount_config_free ( struct mount_config* p_mount ) {
   if ( p_mount != NULL ) {
      mount_config_free_members ( p_mount );
      x_free ( p_mount );
   }
}

void mount_config_ptr_free ( struct mount_config** const pp_mount ) {
   if ( pp_mount != NULL ) {
      mount_config_free ( *pp_mount );
      *pp_mount = NULL;
   }
}

int mount_config_assign_opts_str (
   struct mount_config* const p_mount,
   const char* const opts_str
) {
   mount_config_free_opts ( p_mount );
   p_mount->flags = 0;

   return parse_mount_opts_str (
      opts_str, &(p_mount->flags), &(p_mount->opts)
   );
}

int umount_from_config (
   const struct mount_config* const p_mount
) {
   errno = 0;
   return umount ( p_mount->target );
}

int domount_from_config (
   const struct mount_config* const p_mount
) {
   int fret;
   int retcode;
   char* source_buf;
   const char* source;


   if ( mount_config_check_valid_verbose ( p_mount ) != 0 ) {
      errno = EINVAL;
      return DOMOUNT_ERR_INVALID;
   }

   if ( p_mount->cfg & MOUNT_CFG_IS_ZRAM ) {
      /* use zram directly instead */
      fprintf ( stderr, "%s\n", "domount_from_config(): cannot mount zram" );
      errno = EINVAL;
      return DOMOUNT_ERR_INVALID;
   }


   retcode = DOMOUNT_SUCCESS;

   if ( retcode == DOMOUNT_SUCCESS ) {
      source     = NULL;
      source_buf = NULL;

      /*
       * need_findfs / need_loop are mutually exclusive,
       * else we'd need to strdup source
       */
      if ( p_mount->cfg & MOUNT_CFG_NEED_FINDFS ) {
         if ( str_startswith ( p_mount->source, "/dev/" ) != NULL ) {
            source = p_mount->source;
         } else {
            fret = findfs ( p_mount->source, &source_buf );
            if ( ( fret != DOMOUNT_SUCCESS ) || (source_buf == NULL) ) {
               retcode = DOMOUNT_ERR_FINDFS;
            }
         }

      } else if ( p_mount->cfg & MOUNT_CFG_NEED_LOOP ) {
         if (
            setup_loop_dev (
               p_mount->source, 0, ( (p_mount->flags & MS_RDONLY) != 0 ),
               &source_buf
            ) != 0
         ) {
            retcode = DOMOUNT_ERR_LOSETUP;
         }
      }

      if ( retcode == DOMOUNT_SUCCESS ) {
         if ( source == NULL ) {
            source = source_buf != NULL ? source_buf : p_mount->source;
         }

         if ( p_mount->cfg & MOUNT_CFG_NEED_EXTERNAL ) {
            fret = stdmount_external (
               source,
               p_mount->target,
               p_mount->fstype,
               p_mount->flags,
               p_mount->opts
            );
            if ( fret != 0 ) { retcode = DOMOUNT_ERR_MOUNT_EXTERN; }

         } else if ( p_mount->flags & MS_BIND ) {
            fret = bind_mount ( source, p_mount->target, 0 );

            if ( fret == 0 ) {
               fret = remount (
                  p_mount->target, p_mount->flags, p_mount->opts
               );
            }

            if ( fret != 0 ) { retcode = DOMOUNT_ERR_MOUNT_INTERN_BIND; }

         } else {
            fret = stdmount (
               source,
               p_mount->target,
               p_mount->fstype,
               p_mount->flags,
               p_mount->opts
            );
            if ( fret != 0 ) { retcode = DOMOUNT_ERR_MOUNT_INTERN; }

         }

         source = NULL;
      }

      x_free ( source_buf );
   }

   return -retcode;
}

int domount_from_config_if_enabled (
   struct mount_config* const p_mount
) {
   if ( mount_config_is_enabled ( p_mount ) != 0 ) { return 1; }
   if ( mount_config_autoset    ( p_mount ) != 0 ) { return -2; }

   return domount_from_config ( p_mount );
}

int mount_config_autoset ( struct mount_config* p_mount ) {
   if ( p_mount == NULL ) { return 0; }

   if ( p_mount->fstype != NULL ) {

      STREQ_SWITCH ( p_mount->fstype,
         "auto",
         "squashfs",
         "tmpfs",
         "devtmpfs",
         "nfs",
         "cifs",
         "ztmpfs",
         "zdisk",
         NULL
      ) {
         case -1:
            break;

         case 0: /* auto */
            mount_config_enable ( p_mount, MOUNT_CFG_NEED_EXTERNAL );
            break;

         case 1: /* squashfs */
            mount_config_enable_flag ( p_mount, MS_RDONLY );
            mount_config_enable (
               p_mount,
               (MOUNT_CFG_NEED_LOOP | MOUNT_CFG_NODEV)
            );
            break;

         case 2: /* tmpfs */
         case 3: /* devtmpfs */
            if ( p_mount->opts == NULL ) {
               MOUNT_CONFIG_SET_OR_RETFAIL(p_mount,opts,"mode=0755");
            }
            if ( p_mount->source == NULL ) {
               MOUNT_CONFIG_SET_OR_RETFAIL(p_mount,source,"tmpfs");
            }
            mount_config_enable ( p_mount, MOUNT_CFG_NODEV );
            break;

         case 4: /* nfs */
         case 5: /* cifs */
            /* afs, ... */
            mount_config_enable (
               p_mount,
               (MOUNT_CFG_NEED_EXTERNAL | MOUNT_CFG_NEED_NETWORK | MOUNT_CFG_NODEV)
            );
            break;

         case 6: /* ztmpfs */
         case 7: /* zram */
            /* do not set a default name here, ztmpfs takes care of this */
            mount_config_enable ( p_mount, MOUNT_CFG_IS_ZRAM );
            break;

         default:
            /* COULDFIX: unmatched streq result, log and/or die */
            ;
      }


      if ( p_mount->source == NULL ) {
         STREQ_SWITCH ( p_mount->fstype,
            "aufs",
/*
            "binfmt_misc",
            "cgroup",
            "configfs",
            "cpuset",
            "debugfs",
*/
            "devpts",
/*
            "fuse",
            "fusectl",
            "hugetlbfs",
*/
            "mqueue",
            "overlay",
/*
            "pipefs",
*/
            "proc",
            "ramfs",
/*
            "rootfs",
            "rpc_pipefs",
            "securityfs",
            "sockfs",
*/
            "sysfs",

            NULL
         ) {
            case -1:
               break;

            default:
               MOUNT_CONFIG_SET_OR_RETFAIL(p_mount,source,p_mount->fstype);
         }
      }
   }

   if ( STR_IS_NOT_EMPTY ( p_mount->source ) ) {
      if (
            ( str_startswith ( p_mount->source, "LABEL=" ) != NULL )
         || ( str_startswith ( p_mount->source, "UUID="  ) != NULL )
         || ( str_startswith ( p_mount->source, "PARTUUID="  ) != NULL )
      ) {
         /*mount_config_disable ( p_mount, MOUNT_CFG_NODEV );*/
         mount_config_enable ( p_mount, MOUNT_CFG_NEED_FINDFS );
      }
   }

   return 0;
}




int remount_ro_from_config ( const struct mount_config* const p_mount ) {
   return remount_ro ( p_mount->target, p_mount->flags, p_mount->opts );
}

int remount_rw_from_config ( const struct mount_config* const p_mount ) {
   return remount_rw ( p_mount->target, p_mount->flags, p_mount->opts );
}

int bind_mount_from_config (
   const struct mount_config* const p_mount, const char* const mp
) {
   return bind_mount ( p_mount->source, mp, p_mount->flags );
}

int bind_mount_ro_from_config (
   const struct mount_config* const p_mount, const char* const mp
) {
   if ( bind_mount_from_config ( p_mount, mp ) != 0 ) { return -1; }
   return bind_remount_ro_from_config ( p_mount, mp );
}

int bind_mount_rw_from_config (
   const struct mount_config* const p_mount, const char* const mp
) {
   if ( bind_mount_from_config ( p_mount, mp ) != 0 ) { return -1; }
   return bind_remount_rw_from_config ( p_mount, mp );
}

int bind_remount_ro_from_config (
   const struct mount_config* const p_mount, const char* const mp
) {
   return remount_ro ( mp, ( p_mount->flags | MS_BIND ), p_mount->opts );
}

int bind_remount_rw_from_config (
   const struct mount_config* const p_mount, const char* const mp
) {
   return remount_rw ( mp, ( p_mount->flags | MS_BIND ), p_mount->opts );
}


static int mount_config_check_valid_verbose (
   const struct mount_config* const p_mount
) {
#define MC__ERR(s) fprintf(stderr,"insufficient mount config data: <%s>\n",s)
   int status;

   status = 0;

   if ( p_mount->source == NULL ) {
      MC__ERR("source");
      status = -1;
   }

   if ( p_mount->target == NULL ) {
      MC__ERR("target");
      status = -2;
   }

   if ( p_mount->fstype == NULL ) {
      MC__ERR("fstype");
      status = -3;
   }

   return status;
#undef MC__ERR
}
