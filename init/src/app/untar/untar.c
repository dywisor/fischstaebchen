/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#include "untar.h"
#include "../../common/mac.h"

enum {
   COMP_NONE,
   COMP_GZIP,
   COMP_BZIP2,
   COMP_XZ,
   COMP_LZO,
   COMP_LZ4,

   COMP_UNDEF
};

static int guess_compression ( const char* const filepath );

static int run_untar_no_decompress (
   const char* const dst_root, const char* const tarball
);

static int run_untar_decompress (
   const char* const* const decompress_argv,
   const char* const dst_root,
   const char* const tarball
);

int untar ( const char* const dst_root, const char* const tarball ) {
   const char* decompress_argv[4];
   int comp;

   comp = guess_compression ( tarball );

   switch (comp) {
      case COMP_NONE:
      case COMP_UNDEF:
         return run_untar_no_decompress ( dst_root, tarball );
   }

   decompress_argv [0] = NULL;
   decompress_argv [1] = "-d";
   decompress_argv [2] = NULL;
   decompress_argv [3] = NULL;

   switch (comp) {
      case COMP_LZ4:
         decompress_argv [0] = "lz4";
         break;

      case COMP_GZIP:
         decompress_argv [0] = "gzip";
         decompress_argv [2] = "-c";
         break;

      case COMP_BZIP2:
         decompress_argv [0] = "bzip2";
         decompress_argv [2] = "-c";
         break;

      case COMP_XZ:
         decompress_argv [0] = "xz";
         decompress_argv [2] = "-c";
         break;

      case COMP_LZO:
         decompress_argv [0] = "lzop";
         decompress_argv [2] = "-c";
         break;

      default:
         return -2;
   }

   return run_untar_decompress ( decompress_argv, dst_root, tarball );
}



static int suffix_to_comp_key ( const char* const suffix ) {
#define SUFFIX_IS(k)  ( strcmp ( suffix, k ) == 0 )

   if (
         SUFFIX_IS ( "gzip" )
      || SUFFIX_IS ( "gz" )
      || SUFFIX_IS ( "tgz" )
   ) {
      return COMP_GZIP;

   } else if (
         SUFFIX_IS ( "bzip2" )
      || SUFFIX_IS ( "bz2" )
      || SUFFIX_IS ( "tbz2" )
   ) {
      return COMP_BZIP2;

   } else if (
         SUFFIX_IS ( "xz" )
      || SUFFIX_IS ( "txz" )
   ) {
      return COMP_XZ;

   } else if (
         SUFFIX_IS ( "lzo" )
      || SUFFIX_IS ( "lzop" )
      || SUFFIX_IS ( "tzo" )
   ) {
      return COMP_LZO;

   } else if (
         SUFFIX_IS ( "lz4" )
   ) {
      return COMP_LZ4;
   }

   return COMP_NONE;
#undef SUFFIX_IS
}

static int guess_compression ( const char* const filepath ) {
   const char* name;
   const char* fext;

   if ( filepath == NULL ) { return COMP_UNDEF; }

   name = strrchr ( filepath, '/' );
   if ( name != NULL ) { name++; } else { name = filepath; }

   if ( *name == '\0' ) { return COMP_UNDEF; }

   fext = strrchr ( name, '.' );
   if ( fext == NULL ) { return COMP_NONE; }
   fext++;

   if ( *fext == '\0') { return COMP_NONE; }

   return suffix_to_comp_key ( fext );
}

static void _child_run_tar ( const int tarball_fd ) {
   const char* argv[7];
   const char* taropts;
   size_t k;

   if ( dup2 ( tarball_fd, STDIN_FILENO ) < 0 ) { _exit(EXIT_FAILURE); }

   taropts = getenv ( "UNTAR_OPTS" );

   k = 0;
   argv[k++] = "tar";
   argv[k++] = "x";
   argv[k++] = "-p";

   if ( (taropts == NULL) || (*taropts == '\0') ) {
      ;
   } else {
      argv[k++] = taropts;
   }

   argv[k++] = "-f";
   argv[k++] = "-";
   /*if ( k >= 7 ) { return -1; }*/
   do { argv[k++] = NULL; } while ( k < 7 );

   errno = 0;
   execvp ( argv[0], (char**) argv );
}

static int _dodir ( const char* const dirpath, const mode_t mode ) {
   errno = 0;
   if ( mkdir ( dirpath, mode ) == 0 ) { return 0; }

   switch (errno) {
      case EEXIST:
         return 0;

      default:
         return -1;
   }
}

static int _mkdirp ( const char* const dirpath, const mode_t mode ) {
   int esav;
   char* workpath;
   char* split_pos;

   if ( (dirpath == NULL) || (*dirpath == '\0') ) { return 0; }

   if ( _dodir ( dirpath, mode ) == 0 ) { return 0; }

   workpath = strdup ( dirpath );
   if ( workpath == NULL ) { return -1; }

   split_pos = strchr ( workpath, '/' );
   while ( split_pos != NULL ) {
      *split_pos = '\0';

      if ( _dodir ( workpath, mode ) != 0 ) {
         esav = errno;
         x_free ( workpath );
         errno = esav;
         return -1;
      }

      *split_pos = '/';
      split_pos  = strchr ( (split_pos + 1), '/' );
   }

   x_free ( workpath );
   return _dodir ( dirpath, mode );
}

static void __attribute__((noreturn)) _tarchild (
   const char* const dst_root, const int tarball_fd
) {
   if ( (dst_root != NULL) && (*dst_root != '\0') ) {
      if (
         _mkdirp (
            dst_root, (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
         )
      ) {
         fprintf(stderr,"mkdirp: %s\n",strerror(errno) );
         /* ignore, chdir will fail if dst_root does not exist */
      }


      errno = 0;
      if ( chdir ( dst_root ) != 0 ) {
         fprintf ( stderr, "chdir %s: %s\n", dst_root, strerror(errno) );
         _exit ( EXIT_FAILURE );
      }
   }

   _child_run_tar ( tarball_fd );

   fprintf ( stderr, "tar-command failed to exec: %s\n", strerror(errno) );
   _exit ( EXIT_FAILURE );
}

static void _child_run_decompress (
   const char* const* const decompress_argv,
   const int tarball_fd,
   const int pipe_out_fd
) {
   /* redirect stdin := tarball_fd */
   if ( dup2 ( tarball_fd, STDIN_FILENO ) < 0 ) { return; }

   /* redirect stdout := comlink write end */
   if ( dup2 ( pipe_out_fd, STDOUT_FILENO ) < 0 ) { return; }

   /* redirect stderr := /dev/null (nonfatal) */
   if ( freopen ( "/dev/null", "a", stderr ) != 0 ) {}

   /* exec decompress prog */
   execvp ( decompress_argv[0], (char**)decompress_argv );

   /*fprintf ( stderr, "decompress exec failed: %s", strerror(errno) );*/
}

static int run_untar_decompress (
   const char* const* const decompress_argv,
   const char* const dst_root,
   const char* const tarball
) {
   int decompress_retcode;
   int tar_retcode;
   int status;
   pid_t tar_pid;
   pid_t decompress_pid;
   pid_t pid;
   int tarball_fd;
   int comlink[2];

   /* open tarball file here */
   errno = 0;
   tarball_fd = open ( tarball, O_RDONLY|O_NOATIME );
   if ( tarball_fd < 0 ) {
      fprintf (
         stderr, "failed to open %s: %s\n", tarball, strerror(errno)
      );
   }

   if ( pipe ( comlink ) != 0 ) {
      close ( tarball_fd );
      fprintf ( stderr, "failed to open pipe\n" );
      return -1;
   }

   tar_retcode        = -1;
   decompress_retcode = -1;

   errno = 0;
   decompress_pid = fork();
   switch (decompress_pid) {
      case -1:
         fprintf ( stderr, "failed to fork: %s\n", strerror(errno) );
         close ( tarball_fd );
         close ( comlink[0] );
         close ( comlink[1] );
         return -1;

      case 0:
         /* decompress child */

         /* close pipe read end */
         close ( comlink[0] );

         _child_run_decompress ( decompress_argv, tarball_fd, comlink[1] );

         /* returns on error only */
         close ( tarball_fd );
         close ( comlink[1] );
         _exit ( EXIT_FAILURE );
   }


   /* parent - now with decompress child process */

   /* close tarball_fd, pipe write end */
   close ( tarball_fd );
   close ( comlink[1] );

   /* verify that decompress child did not die prematurely */
   switch ( waitpid ( decompress_pid, &status, WNOHANG ) ) {
      case -1:
         close ( comlink[0] );
         return -1;

      case 0:
         break;

      default:
         /* decompress child already done */
         decompress_retcode = WEXITSTATUS ( status );
         decompress_pid     = 0;

         if ( decompress_retcode != 0 ) {
            /* decompress failed, don't spawn tar process */
            fprintf (
               stderr, "decompressor returned %d\n", decompress_retcode
            );
            close ( comlink[0] );
            return decompress_retcode;
         }
         break;
   }

   /* fork again, tar child */
   errno   = 0;
   tar_pid = fork();
   switch (tar_pid) {
      case -1:
         fprintf ( stderr, "failed to fork: %s\n", strerror(errno) );
         close ( comlink[0] );
         return -1;

      case 0:
         _tarchild ( dst_root, comlink[0] );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunreachable-code"
         close ( comlink[0] );
#pragma GCC diagnostic pop
         _exit(EXIT_FAILURE);
   }

   /* parent - now with decompress + tar child processes */

   /* close pipe read end */
   close ( comlink[0] );

   /* loop until both process done */
   while ( decompress_pid || tar_pid ) {
      pid = waitpid ( -1, &status, 0 );
      switch (pid) {
         case 0:
            break;

         case -1:
            /* kill them! */
            if ( decompress_pid != 0 ) {
               kill ( decompress_pid, SIGTERM );
               kill ( decompress_pid, SIGKILL );
            }
            if ( tar_pid != 0 ) {
               kill ( tar_pid, SIGTERM );
               kill ( tar_pid, SIGKILL );
            }

            return -1;

         default:
            /* pid != 0, we've checked for that already */
            /* FIXME: WIFTCONTINUED() */

            if ( pid == decompress_pid ) {
               decompress_retcode = WEXITSTATUS ( status );
               decompress_pid     = 0;

               /* kill tar process on error */
               if ( decompress_retcode != 0 ) {
                  if ( tar_pid != 0 ) {
                     kill ( tar_pid, SIGTERM ); /* don't check racy retcode */
                     kill ( tar_pid, SIGKILL ); /* don't check racy retcode */
                  }
               }

            } else if ( pid == tar_pid ) {
               tar_retcode = WEXITSTATUS ( status );
               tar_pid     = 0;

               /* kill decompress process on error */
               if ( tar_retcode != 0 ) {
                  if ( decompress_pid != 0 ) {
                     kill ( decompress_pid, SIGTERM ); /* don't check racy retcode */
                     kill ( decompress_pid, SIGKILL ); /* don't check racy retcode */
                  }
               }
            }
            /* else don't care */

            break;
      }
   }

   return (tar_retcode == 0) ? decompress_retcode : tar_retcode;
}


static int run_untar_no_decompress (
   const char* const dst_root, const char* const tarball
) {
   int retcode;
   pid_t tar_pid;
   int tarball_fd;
   int status;

   errno   = 0;
   tar_pid = fork();
   retcode = -1;

   switch (tar_pid) {
      case -1:
         fprintf ( stderr, "failed to fork: %s\n", strerror(errno) );
         return -1;

      case 0:
         errno = 0;
         tarball_fd = open ( tarball, O_RDONLY|O_NOATIME );
         if ( tarball_fd < 0 ) {
            fprintf (
               stderr, "failed to open %s: %s\n", tarball, strerror(errno)
            );
         } else {
            _tarchild ( dst_root, tarball_fd );
         }
         _exit ( EXIT_FAILURE );

      default:
         if ( waitpid ( tar_pid, &status, 0 ) < 0 ) {
            retcode = -1;
         } else {
            retcode = WEXITSTATUS ( status );
         }
         break; /* nop */
   }

   return retcode;
}
