/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>

#include <stdio.h>

#include "argsplit.h"
#include "split.h"
#include "../data_types/dynarray.h"
#include "../data_types/dynstr.h"


int argsplit_str_do (
   const char* const str,
   const char* const arg_delimiters,
   const char* const key_value_delimiter,
   const char* const value_delimiters,
   argsplit_callback arg_callback,
   void* callback_data
) {
   int retcode;
   struct dynarray darr;

   if ( dynarray_init ( &darr, 8 ) != 0 ) { return -1; }

   if ( str_split ( &darr, str, NULL, arg_delimiters, -1 ) < 0 ) {
      dynarray_free ( &darr );
      return -1;
   }

   retcode = argsplit_dynarray_do (
      &darr,
      key_value_delimiter, value_delimiters,
      arg_callback, callback_data
   );

   dynarray_free ( &darr );
   return retcode;
}

int argsplit_dynarray_do (
   struct dynarray* const p_darr,
   const char* const key_value_delimiter,
   const char* const value_delimiters,
   argsplit_callback arg_callback,
   void* callback_data
) {
   return argsplit_argv_do (
      dynarray_argc(p_darr), dynarray_const_argv(p_darr),
      key_value_delimiter, value_delimiters,
      arg_callback, callback_data
   );
}

static int _argsplit_process_arg (
   int* const              flow_control,
   struct dynarray* const  arg_arr,
   const char* const       arg_str,
   argsplit_callback       arg_callback,
   void*                   callback_data
);

int argsplit_argv_do (
   const size_t argc,
   const char* const* const argv,
   const char* const key_value_delimiter,
   const char* const value_delimiters,
   argsplit_callback arg_callback,
   void* callback_data
) {
   int flow_control;
   int retcode;
   size_t k;
   ssize_t nargs;
   struct dynarray arg_arr;

   if ( dynarray_init ( &arg_arr, 8 ) != 0 ) { return -1; }

   retcode      = 0;
   flow_control = ARGSPLIT_KEEP_GOING;

   for (
      k = 0;
      (k < argc) && (flow_control == ARGSPLIT_KEEP_GOING) && (retcode == 0);
      k++
   ) {
      if ( argv[k] != NULL ) {
         nargs = str_split (
            &arg_arr, argv[k], key_value_delimiter, value_delimiters, -1
         );

         if ( nargs < 0 ) {
            retcode = -1;

         } else if ( nargs > 0 ) {
            retcode = _argsplit_process_arg (
               &flow_control,
               &arg_arr, argv[k],
               arg_callback, callback_data
            );
         }

         dynarray_release ( &arg_arr );
      }

   }

   dynarray_free ( &arg_arr );
   return retcode;
}

static int _argsplit_process_arg (
   int* const              flow_control,
   struct dynarray* const  arg_arr,
   const char* const       arg_str,
   argsplit_callback       arg_callback,
   void*                   callback_data
) {
   int    ret;
   size_t iter;
   struct argsplit_data asplit = {
      .str        = arg_str,
      .key        = dynarray_get_str(arg_arr,0),
      .value      = NULL,
      .nargs      = dynarray_argc(arg_arr),
      .argv       = dynarray_const_argv(arg_arr),
      .argno      = 0,
      .end_of_arg = -1
   };

   ret = 0;

   /* always call arg_callback() with value == NULL (as delimiter) */
   *flow_control = ARGSPLIT_KEEP_GOING;
   ret = arg_callback ( &asplit, flow_control, callback_data );

   /*
    * arg_callback() receives const argsplit_data,
    * no need to set end_of_arg in the for loop body
    */
   asplit.end_of_arg = 0;

   for (
      iter = 1;
      (
         (iter < dynarray_argc(arg_arr))
         && (ret == 0)
         && (*flow_control == ARGSPLIT_KEEP_GOING)
      );
      iter++
   ) {
      asplit.value  = dynarray_get_str(arg_arr,iter);
      asplit.nargs  = dynarray_argc(arg_arr) - iter;
      asplit.argv   = dynarray_const_argv_shift(arg_arr,iter);
      asplit.argno  = iter;

      ret = arg_callback ( &asplit, flow_control, callback_data );
   }

   if ( (ret == 0) && (*flow_control == ARGSPLIT_KEEP_GOING) ) {
      asplit.value      = NULL;
      asplit.nargs      = 0;
      asplit.argv       = NULL;
      asplit.argno      = 0;
      asplit.end_of_arg = 1;

      ret = arg_callback ( &asplit, flow_control, callback_data );
   }

   switch (*flow_control) {
      case ARGSPLIT_NEXT_ARG:
         *flow_control = ARGSPLIT_KEEP_GOING;
         break;
   }

   return ret;
}
