/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>

#include "dynarray.h"
#include "dynstr.h"
#include "str_dynarray.h"
#include "mac.h"


int dynarray_append_dynstr_unref (
    struct dynarray* const p_darr,
    struct dynstr_data* const dstr
) {
    char* str_data;

    if ( dynstr_data_append_null ( dstr ) != 0 ) { return -1; }

    dynstr_data_unref ( dstr, &str_data );
    if ( str_data == NULL ) { return -1; }

    if ( dynarray_append_strnodup ( p_darr, str_data ) != 0 ) {
        x_free ( str_data );
        return -1;
    }

    return 0;
}
