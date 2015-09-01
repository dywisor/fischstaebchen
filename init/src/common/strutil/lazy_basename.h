/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_LAZY_BASENAME_H_
#define _COMMON_STRUTIL_LAZY_BASENAME_H_

/**
 * A non-standard conforming basename function that
 * does not modify its input str and returns a pointer
 * to the basename substring.
 *
 * @param input_str   input str, may be NULL (but not useful)
 *
 * @return substring of input_str or pointer to empty str
 */
const char* lazy_basename ( const char* const input_str );

#endif  /* _COMMON_STRUTIL_LAZY_BASENAME_H_ */
