/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_FORMAT_H_
#define _COMMON_STRUTIL_FORMAT_H_

#include <stdlib.h>
#include <stdarg.h>

enum {
    STRFORMAT_RET_SUCCESS = 0,
    STRFORMAT_RET_BUF_TO_SMALL,
    STRFORMAT_RET_STRLEN_NOT_IN_RANGE
};

/**
 * A vsnprintf variant that (re-)interprets the return value.
 *
 * @param str_len_out      pointer for storing the length of the formatted str
 *                         (may be NULL)
 * @param str              buffer for storing the formatted str
 * @param size             buffer size
 * @param format           format str
 * @param ap               format args
 *
 * @return  < 0 if errors occurred,
 *          and a STRFORMAT_RET_ value otherwise (in particular 0 for success)
 */
int str_vformat (
    size_t* const restrict      str_len_out,
    char* const restrict        str,
    const size_t                size,
    const char* const restrict  format,
    va_list ap
);

/** var-arg list variant of str_vformat(). */
__attribute__((sentinel))
int _str_format (
    size_t* const restrict      str_len_out,
    char* const restrict        str,
    const size_t                size,
    const char* const restrict  format,
    ...
);

/** Macro that appends a NULL to the var-arg list passed to _str_format(). */
#define str_format(str_len_out, str, size, ...)  \
    _str_format(str_len_out, str, size, __VA_ARGS__, NULL)

/** Same as str_format(), but passes NULL as str_len_out to _str_format() */
#define str_format__nolen(str, size, ...)  \
    str_format(NULL, str, size, __VA_ARGS__)

/**
 * Same as str_format__nolen(), but converts the return value to a "bool".
 *
 * @param str              buffer for storing the formatted str
 * @param size             buffer size
 * @param format           format str
 * @param ...              format args (trailing sentinel NULL not required)
 *
 * @return  1 (or != 0) if formatting succeeded, 0 if not
 */
#define str_format_check_success(str, size, ...)  \
    (str_format__nolen(str, size, __VA_ARGS__) == STRFORMAT_RET_SUCCESS)

/**
 * Same as str_format__nolen(), but converts the return value to a "bool".
 *
 * @param str              buffer for storing the formatted str
 * @param size             buffer size
 * @param format           format str
 * @param ...              format args (trailing sentinel NULL not required)
 *
 * @return  0 if formatting succeeded, and 1 (!= 0) otherwise
 */
#define str_format_check_fail(str, size, ...)  \
    (str_format__nolen(str, size, __VA_ARGS__) != STRFORMAT_RET_SUCCESS)


#endif  /* _COMMON_STRUTIL_FORMAT_H_ */
