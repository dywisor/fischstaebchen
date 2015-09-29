/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MAC_H_
#define _COMMON_MAC_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

#include "config.h"


#ifndef UNUSED
#define UNUSED(d)  d __attribute__((unused))
#endif

#define DEBUG_SLEEP_DURATION  3

#define DBG_SLEEPF(fmt,...)  \
   do { \
      if ( fmt != NULL ) { \
         fprintf ( stderr, fmt, __VA_ARGS__ ); \
      } \
      if ( DEBUG_SLEEP_DURATION > 0 ) { \
         sleep ( DEBUG_SLEEP_DURATION ); \
      } \
   } while (0)

#define DBG_SLEEP(s)  DBG_SLEEPF(">>> DBG_SLEEP: %s <<<\n", s)


#define RETFAIL_CODE  -1

#define B_TRUE  1
#define B_FALSE 0
#define X_TRUE  0
#define X_FALSE 1

#define DO_ONCE(code) do { code } while(0)
#define DO_WHILE_ONCE(code)  DO_ONCE(code)

#define RET_NULL_IFNOT_MALLOC(var,size)  DO_ONCE ( \
   if ( ( var = malloc(size) ) == NULL ) { return NULL; } )

#define RET_NULL_IFNOT_MALLOC_VAR(var)   RET_NULL_IFNOT_MALLOC(var,sizeof *var)

/*
 * do not enable FREE_CHECK_NULL if you don't need it,
 * it may add redundant NULL checks
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#if FREE_CHECK_NULL
#pragma GCC diagnostic pop

#define x_free(ptr)   DO_ONCE ( \
   { if ( ptr != NULL ) { free(ptr); ptr = NULL; } } )
#else
#define x_free(ptr)   DO_ONCE ( { free(ptr); ptr = NULL; } )
#endif

#define x_free_arr_items(arr,len)  \
   DO_ONCE ( \
      if ( arr != NULL ) { \
         size_t iter; \
         for ( iter = 0; iter < len; iter++ ) { \
            x_free ( arr[iter] ); \
         } \
      } \
   )


#define IGNORE_RETCODE(code)  \
   DO_ONCE ( if ( code ) { ; } )

#define RET_IF_B_TRUE(rc,code)  \
   DO_ONCE ( \
      if ( ( code ) != 0 ) { return rc; } )

#define RET_IF_B_FALSE(rc,code)  \
   DO_ONCE ( \
      if ( ( code ) == 0 ) { return rc; } )

#define RET_IF_X_TRUE(rc,code)    RET_IF_B_FALSE ( rc, code )
#define RET_IF_X_FALSE(rc,code)   RET_IF_B_TRUE  ( rc, code )


#define RETFAIL_IF_B_TRUE(code)    RET_IF_B_TRUE  ( RETFAIL_CODE, code )
#define RETFAIL_IF_B_FALSE(code)   RET_IF_B_FALSE ( RETFAIL_CODE, code )
#define RETFAIL_IF_X_TRUE(code)    RET_IF_X_TRUE  ( RETFAIL_CODE, code )
#define RETFAIL_IF_X_FALSE(code)   RET_IF_X_FALSE ( RETFAIL_CODE, code )

#define RET_VOID_IF_NULL(code)  \
   DO_ONCE ( \
      if ( ( code ) == NULL ) { return; } \
   )

#define RET_CODE_IF_NULL(rc,code)  \
   RET_IF_B_TRUE ( rc, ( ( code ) == NULL ) )

#define RETFAIL_IF_NULL(code) RET_CODE_IF_NULL ( RETFAIL_CODE, code )



/* file I/O */
#define X_DECLARE_FILE_READ_BUFFER(buf_varname)  \
   char buf_varname[DEFAULT_FILE_READ_BUFFER_SIZE]

#define X_FILE_READ_BUFFER(fd, buf_varname)  \
   read(fd, buf_varname, DEFAULT_FILE_READ_BUFFER_SIZE)

#define X_FILE_READ_BUFFER_NBUF(fd, buf_varname, nbuf_varname)  \
   (nbuf_varname = X_FILE_READ_BUFFER(fd, buf_varname))

#define X_WITH_OPEN_FILE_CALL_FUNC(retcode_var, filepath, mode, func_call) \
    do { \
        int fd; \
        \
        if ( filepath == NULL ) { \
            retcode_var = -1; \
        } else { \
            fd = open ( filepath, mode ); \
            if ( fd < 0 ) { \
                retcode_var = -1; \
            } else { \
                retcode_var = func_call; \
                IGNORE_RETCODE ( close ( fd ) ); \
            } \
        } \
    } while (0)


#define X_PRESERVE_ERRNO_AS(bak_varname, code)  \
    do { \
        const int bak_varname = errno; \
        code; \
    } while (0)

#define X_PRESERVE_ERRNO(...)  X_PRESERVE_ERRNO_AS(esav, __VA_ARGS__)

#endif /* _COMMON_MAC_H_ */
