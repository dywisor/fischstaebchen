/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_JOIN_H_
#define _COMMON_STRUTIL_JOIN_H_

#include <stdlib.h>

#include "../dynarray.h"

/**
 * Concatenates an arbitrary number of strings.
 *
 * @param seq         str sequence to be inserted between each str pair,
 *                     may be NULL or empty('\0') for simple concatenation
 * @param <va_list>   var-args str list, must be NULL-terminated
 *
 * @return concatenated str or NULL on error
 */
char* _join_str ( const char* const seq, ... );
#define join_str(seq,...)  _join_str(seq,__VA_ARGS__,NULL)

/**
 * Concatenates strings from the given array. Skips NULL items.
 *
 * @param seq          str sequence to be inserted between each str pair,
  *                     may be NULL or empty('\0') for simple concatenation
 * @param arr_len      length of the array
 * @param arr          array of strings
 *
 * @return concatenated str or NULL on error
 */
char* join_str_array (
   const char* const seq,
   const size_t arr_len,
   const char* const* const arr
);

/**
 * Concatenates strings from the given dynarray. Skips NULL items.
 *
 * @param seq          str sequence to be inserted between each str pair,
  *                     may be NULL or empty('\0') for simple concatenation
 * @param p_darr       str dynarray
 *
 * @return concatenated str or NULL on error
 */
char* join_str_dynarray (
   const char* const seq,
   const struct dynarray* const p_darr
);

/**
 * Joins two strings.
 *
 * @param left    str #1, may be NULL
 * @param right   str #2, may be NULL
 *
 * @return concatenated str; NULL on error or if both input strings were NULL
*/
char* join_str_pair ( const char* const left, const char* const right );

/**
 * Joins three strings.
 *
 * @param left    str #1, may be NULL
 * @param middle  str #2, may be NULL
 * @param right   str #3, may be NULL
 *
 * @return concatenated str; NULL on error or if all input strings were NULL
 */
char* join_str_triple (
   const char* const left, const char* const middle, const char* const right
);

#endif /* _COMMON_STRUTIL_JOIN_H_ */
