/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_MOUNT_CONFIG_H_
#define _COMMON_FS_MOUNT_CONFIG_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/*#include "../mac.h"*/

/** domount error codes */
enum {
   DOMOUNT_SUCCESS     = 0x0,
   DOMOUNT_ERR_UNKNOWN = 0x1,
   DOMOUNT_ERR_INVALID,
   DOMOUNT_ERR_FINDFS,
   DOMOUNT_ERR_LOSETUP,
   DOMOUNT_ERR_MOUNT_INTERN,
   DOMOUNT_ERR_MOUNT_INTERN_BIND,
   DOMOUNT_ERR_MOUNT_EXTERN
};

/** mount_config cfg flags */
enum {
   MOUNT_CFG_NONE          = 0x0,
   MOUNT_CFG_NEED_EXTERNAL = 0x1,
   MOUNT_CFG_IS_ENABLED    = 0x2,
   MOUNT_CFG_NEED_FINDFS   = 0x4,
   MOUNT_CFG_NEED_NETWORK  = 0x8,
   MOUNT_CFG_NEED_LOOP     = 0x10,
   MOUNT_CFG_NODEV         = 0x20, /** write-only flag */
   /**
    * zram is not supported by domount(), use zram/tmpfs.h
    *
    * However, mount_config_autoset() understands "zram" and "ztmpfs"
    * as filesystem types and sets this flag accordingly.
    */
   MOUNT_CFG_IS_ZRAM       = 0x40,

   MOUNT_CFG__ALL /* ? remove/rename this. */
};

struct mount_config {
   /** extra config flags
    * (not to be confused with "flags" below,
    *  which is what gets passed as flags to mount(2))
    */
   unsigned long  cfg;
   char*          source;
   char*          target;
   char*          fstype;
   unsigned long  flags;
   char*          opts;
};

/**
 * Returns a string describing a mount error code.
 *
 * @param mnterr  error code
 *
 * @return const char*
 */
const char* domount_strerror ( const int mnterr );

/**
 * Macro that a mount_config attribute.
 *
 * @param p   (struct mount_config*)
 * @param m   attribute name
 * @param v   value
 *
 * Aborts the current function (i.e. returns -1) on error.
 */
#define MOUNT_CONFIG_SET_OR_RETFAIL(p,m,v)  \
   do { \
      if ( (mount_config_set_ ## m)(p,v) != 0 ) { return -1; } \
   } while(0)

/**
 * Macro that initializes a struct mount_config*.
 *
 * @param p          (struct mount_config*)
 * @param <va_args>  see mount_config_init()
 *
 * Aborts the current function (i.e. returns -1) on error.
 */
#define MOUNT_CONFIG_INIT_OR_RETFAIL(p,...) \
   do { \
      switch ( mount_config_ptr_init ( &p, __VA_ARGS__ ) ) { \
         case 0: \
         case 1: \
            break; \
         default: \
            return -1; \
      } \
   } while(0)

/**
 * Macro that "zero-"initializes a struct mount_config*.
 *
 * @param p          (struct mount_config*)
 *
 * Aborts the current function (i.e. returns -1) on error.
 */
#define MOUNT_CONFIG_NULL_INIT_OR_RETFAIL(p) \
   MOUNT_CONFIG_INIT_OR_RETFAIL ( p, NULL, NULL, NULL, 0, NULL, 0 )

/**
 * Macro that mostly "zero-"initializes a struct mount_config*.
 *
 * @param p          (struct mount_config*)
 * @param mp         (const char*) mountpoint
 *
 * Aborts the current function (i.e. returns -1) on error.
 */
#define MOUNT_CONFIG_MP_INIT_OR_RETFAIL(p,mp) \
   MOUNT_CONFIG_INIT_OR_RETFAIL ( p, NULL, mp, NULL, 0, NULL, 0 )

/**
 * Initializes the attributes of a struct mount_config*.
 *
 * @param p_mount   (should not be NULL)
 * @param source    (may be NULL)
 * @param target    (may be NULL)
 * @param fstype    (may be NULL)
 * @param flags
 * @param opts      (may be NULL)
 * @param cfg
 *
 * @return 0 on success, else -1
 */
__attribute__((warn_unused_result))
int mount_config_init (
   struct mount_config* const p_mount,
   const char* const  source,
   const char* const  target,
   const char* const  fstype,
   unsigned long      flags,
   const char* const  opts,
   unsigned long      cfg
);

/**
 * Initializes a struct mount_config** (including its attributes).
 *
 * @param pp_mount  (should not be NULL, *pp_mount must be NULL)
 * @param source    (may be NULL)
 * @param target    (may be NULL)
 * @param fstype    (may be NULL)
 * @param flags
 * @param opts      (may be NULL)
 * @param cfg
 *
 * @return 0 on success, else -1
 */
__attribute__((warn_unused_result))
int mount_config_ptr_init (
   struct mount_config** const pp_mount,
   const char* const  source,
   const char* const  target,
   const char* const  fstype,
   unsigned long      flags,
   const char* const  opts,
   unsigned long      cfg
);

/**
 * Frees a struct mount_config* (incl. its attributes).
 *
 * @param p_mount (may be NULL)
 */
void mount_config_free     ( struct mount_config* p_mount );

/**
 * Frees a struct mount_config** and its attributes.
 *
 * @param pp_mount (may be NULL)
 */
void mount_config_ptr_free ( struct mount_config** const pp_mount );

/* set_member/free_member functions */

/**
 * Frees all attributes of a struct mount_config*.
 *
 * @param p_mount (may be NULL)
 */
void mount_config_free_members (
   struct mount_config* const p_mount
);

/** Frees the 'source' attribute of a struct mount_config*. */
void mount_config_free_source ( struct mount_config* const );

int mount_config_set_source (
   struct mount_config* const, const char* const
);

/** Frees the 'target' attribute of a struct mount_config*. */
void mount_config_free_target ( struct mount_config* const );

int mount_config_set_target (
   struct mount_config* const, const char* const
);

/** Frees the 'fstype' attribute of a struct mount_config*. */
void mount_config_free_fstype ( struct mount_config* const );
int mount_config_set_fstype (
   struct mount_config* const, const char* const
);

/** Frees the 'opts' attribute of a struct mount_config*. */
void mount_config_free_opts ( struct mount_config* const );
int mount_config_set_opts (
   struct mount_config* const, const char* const
);

/** Parses a mount opts str and sets the flags and opts attributes. */
int mount_config_assign_opts_str (
   struct mount_config* const p_mount,
   const char* const opts_str
);

/** Enables one or more mount_config flags */
static inline void mount_config_enable (
   struct mount_config* const p_mount, unsigned long cfg
) {
   p_mount->cfg |= cfg;
}

/** Disables one or more mount_config flags */
static inline void mount_config_disable (
   struct mount_config* const p_mount, unsigned long cfg
) {
   p_mount->cfg &= ~cfg;
}

/** Enables one or more mount flags */
static inline void mount_config_enable_flag (
   struct mount_config* const p_mount, unsigned long flags
) {
   p_mount->flags |= flags;
}

/** Disables one or more mount flags */
static inline void mount_config_disable_flag (
   struct mount_config* const p_mount, unsigned long flags
) {
   p_mount->flags &= ~flags;
}



static inline int mount_config_is_enabled (
   const struct mount_config* const p_mount
) {
   return (
      (p_mount != NULL) && (p_mount->cfg & MOUNT_CFG_IS_ENABLED)
   ) ? 0 : 1;
}

/**
 * Returns 0 if the given struct mount_config* is valid, i.e. has enough data.
 *
 * Note that this function does not check whether domount() can handle the
 * mount config (e.g. MOUNT_CFG_IS_ZRAM).
 *
 * @param p_mount
 *
 * @return 0 on success, else non-zero
 */
static inline int mount_config_is_valid (
   const struct mount_config* const p_mount
) {
   return (
      ( p_mount->source != NULL ) &&
      ( p_mount->target != NULL ) &&
      ( p_mount->fstype != NULL )
   ) ? 0 : 1;
}

/**
 * Calls domount_from_config(p_mount) if p_mount is enabled.
 * Also takes care of mount_config_autoset().
 */
int domount_from_config_if_enabled (
   struct mount_config* const p_mount
);

/**
 * Performs a mount action as specified by the given mount config:
 *
 *  - checks that p_mount is valid
 *  - checks additional config flags (zram)
 *  - runs findfs (if necessary)
 *  - sets up a loop device (if necessary)
 *  - calls stdmount() or stdmount_external(),
 *     depending on whether MOUNT_CFG_NEED_EXTERNAL is set
 *
 * @param p_mount  mount config
 *
 * @return 0 on success, else mount config error code (use domount_strerror())
 */
int domount_from_config (
   const struct mount_config* const p_mount
);

/**
 * Unmounts a mount_config.
 * This is basically the same as umount(p_mount->target).
 */
int umount_from_config (
   const struct mount_config* const p_mount
);

/** Remounts an already mounted mount_config in readonly mode. */
int remount_ro_from_config ( const struct mount_config* const p_mount );

/** Remounts an already mounted mount_config in read-write mode. */
int remount_rw_from_config ( const struct mount_config* const p_mount );

int bind_mount_from_config (
   const struct mount_config* const p_mount, const char* const mp
);

int bind_mount_ro_from_config (
   const struct mount_config* const p_mount, const char* const mp
);

int bind_mount_rw_from_config (
   const struct mount_config* const p_mount, const char* const mp
);

int bind_remount_ro_from_config (
   const struct mount_config* const p_mount, const char* const mp
);

int bind_remount_rw_from_config (
   const struct mount_config* const p_mount, const char* const mp
);

/**
 * Fills unset attributes and sets additional config flags.
 * Should be called manually before domount_from_config().
 *
 * @param p_mount
 *
 * @return -1 on error, else 0
 */
int mount_config_autoset ( struct mount_config* p_mount );

#endif /* _COMMON_FS_MOUNT_CONFIG_H_ */
