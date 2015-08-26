/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>

#include "../base.h"
#include "../initramfs.h"
#include "../globals.h"
#include "../config.h"
#include "../baselayout.h"
#include "../hacks.h"
#include "../premount.h"
#include "../../common/mac.h"
#include "../../common/dynarray.h"
#include "../../common/misc/env.h"
#include "../../common/strutil/compare.h"
#include "../../common/strutil/join.h"
#include "../../common/fs/fileio.h"



#define IRUN(...)  INITRAMFS_FAIL_LATCH(__VA_ARGS__)
#define IRUN_HOOK(phase,is_critical)  \
   IRUN ( \
      (phase " hook"), \
      initramfs_run_hook ( phase, is_critical, &found_hook ) )

#define IRUN_CRITICAL_HOOK(phase)  IRUN_HOOK(phase,1)



static int _initramfs_hacks_override_init_prog (
   const char* const read_from
) {
   if ( access ( read_from, F_OK ) == 0 ) {
      if ( initramfs_globals->real_init != NULL ) {
         x_free ( initramfs_globals->real_init );
      }

      return read_sysfs_file ( read_from, &(initramfs_globals->real_init) );
   }
   return 0;
}

static int has_flagfile ( const char* const name ) {
   int   retcode;
   char* flagfile;

   RETFAIL_IF_NULL (
      flagfile = join_str_pair ( ( INITRAMFS_RUN_CONFDIR "/" ), name )
   );

   retcode = ( access ( flagfile, F_OK ) == 0 );

   x_free ( flagfile );
   return retcode;
}

static int have_modules_dir (void) {
   char* mdir;
   int   retcode;

   RETFAIL_IF_STR_IS_EMPTY ( initramfs_globals->kernel_release_str );

   RETFAIL_IF_NULL (
      mdir = join_str_triple (
         (INITRAMFS_KERNEL_MODULES_DIR "/"),
         initramfs_globals->kernel_release_str, "/modules.dep"
      )
   );

   retcode = access ( mdir, F_OK );
   x_free ( mdir );

   return retcode;
}

static int load_modules ( const struct dynarray* const modules ) {
   size_t k;

   for ( k = 0; k < dynarray_argc(modules); k++ ) {
      _INITRAMFS_FAIL_LATCH (
         "    *", dynarray_get_str ( modules, k ),
         initramfs_modprobe ( 0, dynarray_get_str ( modules, k ) )
      );
   }
   return 0;
}

static int main_init (void) {
   int found_hook;
   int did_load_modules;

   if ( default_initramfs_start() != 0 ) { return EXIT_FAILURE; }

   IRUN ( "init rundir", initramfs_init_rundir() );

   initramfs_additional_env_exports();

   /* phase-out cmdline parse / env setup */
   IRUN_CRITICAL_HOOK ( "early-env-setup" );
   IRUN_CRITICAL_HOOK ( "parse-cmdline" );
   IRUN_CRITICAL_HOOK ( "env-setup" );

   /*
    * try to load modules early (needs modules in a cpio archive)
    *
    *  (obviously doesn't work when the modules dir is not /lib/modules,
    *   but we'll try to load modules unconditionally later on anyway)
    *
    */
   did_load_modules = 0;
   if (
         ( initramfs_globals->load_modules->len > 0 )
      && ( have_modules_dir() == 0 )
   ) {
      did_load_modules = (
         load_modules ( initramfs_globals->load_modules ) == 0
      );
   }

   /* phase-out devfs setup */
   IRUN_CRITICAL_HOOK ( "devfs-setup" );

   /* phase-out net-setup */
   IRUN_CRITICAL_HOOK ( "net-setup" );

   if ( initramfs_run_hook ( "net-ifup", 1, &found_hook ) == 0 ) {
      if (
         ( found_hook != 0 ) && ( has_flagfile ( "have-net" ) > 0 )
      ) {
         initramfs_globals->have_network = 1;
      } else {
         initramfs_globals->have_network = 0;
      }

   } else {
      initramfs_globals->have_network = 0;
   }
   export_env_shbool ( "HAVE_NET", initramfs_globals->have_network );
   IRUN_CRITICAL_HOOK ( "post-net" );

   /* load modules if not already done
    *  this allows to load a module tarball via http and whatnot
    */
   if (
      ( did_load_modules == 0 )
      && ( initramfs_globals->load_modules->len > 0 )
   ) {
      IRUN (
         "load modules", load_modules ( initramfs_globals->load_modules )
      );
   }
   /* no longer needed */
   initramfs_globals_free_modprobe();

   IRUN ( "initramfs mounts", initramfs_premount("base") );

   /* phase-out pre-rootmount */
   /* fsck! */
   IRUN_CRITICAL_HOOK ( "pre-mount-newroot" );

   IRUN ( "early initramfs aux mounts", aux_premount("early") );

   if ( has_flagfile ( "no-mount-root" ) < 1 ) {
      IRUN ( "mount / in newroot", initramfs_mount_newroot_root() );
   } else {
      initramfs_debug (
         "not using builtin %s: disabled by flagfile.", "\n", "mount-root"
      );
   }

   /* phase-out rootmount */
   IRUN_CRITICAL_HOOK ( "mount-newroot" );

   if (
      ( has_flagfile ( "no-mount-usr" ) < 1 ) &&
      ( has_flagfile ( "want-squashed-usr" ) < 1 )
   ) {
      IRUN ( "mount /usr in newroot", initramfs_mount_newroot_usr() );
   } else {
      initramfs_debug (
         "not using builtin %s: disabled by flagfile.", "\n", "mount-usr"
      );
   }

   IRUN ( "newroot mounts", newroot_premount("base") );

   /* phase-out post-rootmount */
   IRUN_CRITICAL_HOOK ( "post-mount-newroot" );

   IRUN ( "initramfs aux mounts", aux_premount("base") );

   /* phase-out subtree mount */
   IRUN_CRITICAL_HOOK ( "subtree-mount" );

   IRUN ( "newroot subtree mounts", newroot_premount("subtree") );

   /* phase-out populate-newroot */
   IRUN_CRITICAL_HOOK ( "populate-newroot" );

   IRUN ( "newroot basemounts", initramfs_newroot_basemounts() );

   if ( has_flagfile ( "no-premount" ) < 1 ) {
      IRUN ( "newroot fstab premount", initramfs_newroot_premount(NULL) );
   } else {
      initramfs_debug (
         "not using builtin %s: disabled by flagfile.", "\n",
         "premount-newroot"
      );
   }

   /* mount /tmp in newroot if not already done */
   if ( has_flagfile ( "no-mount-tmpdir" ) < 1 ) {
      IRUN ( "newroot mount tmpdir", initramfs_mount_newroot_tmpdir() );
   } else {
      initramfs_debug (
         "not using builtin %s: disabled by flagfile.", "\n",
         "mount-tmpdir"
      );
   }

   /* set up user tmpdirs
    * (per-uid, with user names symlinked to the uid dirs)
    * */
   if ( has_flagfile ( "no-user-tmpdirs" ) < 1 ) {
      IRUN ( "newroot tmpdir setup", initramfs_setup_newroot_tmpdir() );
   } else {
      initramfs_debug (
         "not using builtin %s: disabled by flagfile.", "\n",
         "user-tmpdirs setup"
      );
   }

   /* phase-out setup-newroot */
   IRUN_CRITICAL_HOOK ( "setup-newroot" );

   /* phase-out finalize-newroot */
   IRUN_CRITICAL_HOOK ( "finalize-newroot" );

   initramfs_eject_aux_mounts();

   /* phase-out ifdown */
   if ( initramfs_globals->have_network ) {
      IRUN_CRITICAL_HOOK ( "net-ifdown" );
      initramfs_globals->have_network = 0;
      export_env_shbool ( "HAVE_NET", 0 );
   }

   IRUN (
      "preswitch shell", initramfs_run_user_preswitch_shell_if_requested()
   );

   IRUN (
      "override-init",
      _initramfs_hacks_override_init_prog (
         GET_INITRAMFS_RUN_CONFDIR("init_prog")
      )
   );

   sync();

   /* COULDFIX: free $$globals (doesn't matter much, exec() ahead...) */
   IRUN ( "switch to newroot", initramfs_switch_root() );

   return 0;
}

#undef IRUN

int main ( int UNUSED(argc), char** UNUSED(argv) ) {
   main_init(); /* does only return on error */
   initramfs_eject_mounts();
   return EXIT_FAILURE;
}
