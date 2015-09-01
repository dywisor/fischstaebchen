/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "format.h"

int _str_format (
    size_t* const restrict      str_len_out,
    char* const restrict        str,
    const size_t                size,
    const char* const restrict  format,
    ...
) {
    int     ret;
    va_list vargs;

    va_start ( vargs, format );
    ret = str_vformat ( str_len_out, str, size, format, vargs );
    va_end ( vargs );

    return ret;
}

int str_vformat (
    size_t* const restrict      str_len_out,
    char* const restrict        str,
    const size_t                size,
    const char* const restrict  format,
    va_list ap
) {
    int    ret;
    size_t slen;

    if ( str_len_out != NULL ) { *str_len_out = 0; }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = vsnprintf ( str, size, format, ap );
#pragma GCC diagnostic pop

    if ( ret < 0 ) {
        return ret;

    } else if ( ret == 0 ) {
        /* 0 chars have been written, including the '\0' byte, wtf? */
        return -1;

    } else {
        /* slen: length of str, w/o '\0' byte (equal to strlen(str)) */
        slen = ret - 1;
        if ( str_len_out != NULL ) { *str_len_out = slen; }

        if ( (slen + 1) >= size ) {
            return STRFORMAT_RET_BUF_TO_SMALL;

        } else {
            return STRFORMAT_RET_SUCCESS;
        }
    }
}





