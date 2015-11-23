/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */


#ifndef _COMMON_DATA_TYPES_DYNSTR_BUFFER_H_
#define _COMMON_DATA_TYPES_DYNSTR_BUFFER_H_


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "dynstr.h"


struct fspath_prefix_buffer {
   size_t prefix_len_sav;
   struct dynstr_data dynstr;
};


void fspath_prefix_buffer_free_data (
   struct fspath_prefix_buffer* const buf
);

void fspath_prefix_buffer_free (
   struct fspath_prefix_buffer** const p_buf
);

int fspath_prefix_buffer_init (
   struct fspath_prefix_buffer** const p_buf,
   const char* const fspath,
   const int is_dirpath
);

int fspath_prefix_buffer_reset (
   struct fspath_prefix_buffer* const buf
);

const char* fspath_prefix_buffer_append (
   struct fspath_prefix_buffer* const buf,
   const char* const relpath
);

const char* fspath_prefix_buffer_set (
   struct fspath_prefix_buffer* const buf,
   const char* const filename
);

const char* fspath_prefix_buffer_peek (
   const struct fspath_prefix_buffer* const buf
);

#endif  /* _COMMON_DATA_TYPES_DYNSTR_BUFFER_H_ */
