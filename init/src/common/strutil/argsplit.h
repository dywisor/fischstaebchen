/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_ARGSPLIT_H_
#define _COMMON_STRUTIL_ARGSPLIT_H_

#include <sys/types.h>

#include "../data_types/dynarray.h"

/** argsplit status */
enum {
   /**
    *  continue with the next element of the current arg
    *  (or the first element of the next arg
    */
   ARGSPLIT_KEEP_GOING,

   /**
    * continue with the first element of the next arg
    */
   ARGSPLIT_NEXT_ARG,

   /**
    * stop parsing
    */
   ARGSPLIT_STOP_PARSE
};

/** data struct that gets passed to argsplit_callback functions */
struct argsplit_data {
   const char*          str;
   const char*          key;
   const char*          value;
   size_t               nargs;
   const char* const*   argv;
   size_t               argno;
   int                  end_of_arg;
};

/** function that gets called by argsplit_{str,argv,dynarray}_do()
 *
 * @param <struct argsplit_data>  argsplit data
 * @param <int*>                  "control flow", see argsplit status above
 * @param <void*>                 callback data
 */
typedef int(*argsplit_callback) (const struct argsplit_data*, int*, void*);

int argsplit_argv_do (
   const size_t argc,
   const char* const* const argv,
   const char* const key_value_delimiter,
   const char* const value_delimiters,
   argsplit_callback arg_callback,
   void* callback_data
);

int argsplit_dynarray_do (
   struct dynarray* const p_darr,
   const char* const key_value_delimiter,
   const char* const value_delimiters,
   argsplit_callback arg_callback,
   void* callback_data
);

int argsplit_str_do (
   const char* const str,
   const char* const arg_delimiters,
   const char* const key_value_delimiter,
   const char* const value_delimiters,
   argsplit_callback arg_callback,
   void* callback_data
);


#endif /* _COMMON_STRUTIL_ARGSPLIT_H_ */
