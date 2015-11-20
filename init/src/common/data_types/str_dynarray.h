/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

/** additional, str-related dynarray functionality */

#ifndef _COMMON_STR_DYNARRAY_H_
#define _COMMON_STR_DYNARRAY_H_

#include "dynarray.h"
#include "dynstr.h"

/**
 * Transfers a str (char *) from a dynstr_data struct to a dynarray struct.
 *
 * The dynstr's str gets either appended to the dynarray,
 * or, if that fails, freed by means of x_free().
 * After calling this function, whether successful or not,
 * the dynstr holds no data and can be used to build up a new str.
 *
 * @param p_darr      dynarray (must not be NULL)
 * @param dstr        dynstr   (must not be NULL, str should not be NULL)
 *
 * @return 0 when successful, else non-zero
 */
int dynarray_append_dynstr_unref (
    struct dynarray* const p_darr,
    struct dynstr_data* const dstr
);

#endif /* _COMMON_STR_DYNARRAY_H_ */
