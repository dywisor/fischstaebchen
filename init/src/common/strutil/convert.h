/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_CONVERT_H_
#define _COMMON_STRUTIL_CONVERT_H_

#include <sys/types.h>
#include <stdint.h>

/**
 * strtol() wrapper that converts the entire str and returns whether the
 * operation succeeded.
 *
 * @param str   input str
 * @param lout  int pointer, used for storing the resulting number (>=32bit)
 *
 * @return 0 on success, else non-zero
 */
int str_to_long ( const char* const str, long* const lout );

/**
 * Similar to str_to_long(), but accepts a base parameter.
 *
 * @param base
 * @param str
 * @param lout
 *
 * @return 0 on success, else non-zero
 */
int str_to_long_base ( int base, const char* const str, long* const lout );

/**
 * Converts a string to its uppercase variant (in-place operation).
 *
 * @param str
 */
void convert_to_uppercase ( char* const str );

/**
 * Converts a copy of the given string to its uppercase variant and returns it.
 *
 * @param str
 *
 * @return converted str or NULL
 */
__attribute__((warn_unused_result))
char* get_uppercase ( const char* const str );

/**
 * Converts a string to its lowercase variant (in-place operation).
 *
 * @param str
 */
void convert_to_lowercase ( char* const str );

/**
 * Converts a copy of the given string to its lowercase variant and returns it.
 *
 * @param str
 *
 * @return converted str or NULL
 */
__attribute__((warn_unused_result))
char* get_lowercase ( const char* const str );


/**
 * Converts a uint8_t value to a str.
 *
 * @param k
 *
 * @return converted str or NULL
 */
__attribute__((warn_unused_result))
char* uint8_to_str ( const uint8_t k );



#endif /* _COMMON_STRUTIL_CONVERT_H_ */
