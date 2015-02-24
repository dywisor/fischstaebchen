/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_MISC_H_
#define _COMMON_STRUTIL_MISC_H_

#include <sys/types.h>

/**
 * Returns the substr of a str starting at the given offset.
 * Offset may be < 0, e.g. str_seek("abc", -1) => "c".
 *
 * @param str
 * @param offset
 *
 * @return substr (pointer to char seq in input str) or NULL on error
 */
const char* str_seek ( const char* const str, const ssize_t offset );

#endif /* _COMMON_STRUTIL_MISC_H_ */
