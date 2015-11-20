/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_SPLIT_H_
#define _COMMON_STRUTIL_SPLIT_H_

#include <sys/types.h>

#include "../data_types/dynarray.h"

/**
 * Splits a str
 *
 * @param p_darr             array for storing the str fields
 * @param str                str to split (should not be NULL)
 * @param initial_delimiter  delimiter that separates the first field
 *                            from the remainder (may be NULL)
 * @param subseq_delimiters  delimiter for all subsequent fields
 *                            (or all fields if initial_delimiter is NULL)
 *                            May be NULL if initial_delimiter is not NULL.
 * @param max_split          ignored (not implemented). Use -1 for "no max".
 *
 * @return -1 on error, else number of fields
 */
ssize_t str_split (
   struct dynarray* const p_darr,
   const char* const str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t max_split
);

/**
 * Splits a str.
 * Unsafe variant of str_split() that modifies the input str.
 *
 * @param p_darr             array for storing the str fields
 * @param work_str           str to split (will be modified) (should not be NULL)
 * @param initial_delimiter  delimiter that separates the first field
 *                            from the remainder (may be NULL)
 * @param subseq_delimiters  delimiter for all subsequent fields
 *                            (or all fields if initial_delimiter is NULL)
 *                            May be NULL if initial_delimiter is not NULL.
 * @param max_split          ignored (not implemented). Use -1 for "no max".
 *
 * @return -1 on error, else number of fields
 */

ssize_t str_split_unsafe (
   struct dynarray* const p_darr,
   char* const work_str,
   const char* const initial_delimiter,
   const char* const subseq_delimiters,
   const ssize_t max_split
);

#endif /* _COMMON_STRUTIL_SPLIT_H_ */
