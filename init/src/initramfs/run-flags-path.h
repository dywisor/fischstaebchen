/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_RUN_FLAGS_PATH_H_
#define _INITRAMFS_RUN_FLAGS_PATH_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "../common/data_types/dynstr_buffer.h"


extern struct fspath_prefix_buffer* \
   _fisch_initramfs_run_flags_flagpath_buffer;



void _fisch_initramfs_run_flags_libfini (void) __attribute__((destructor));


char* ishare_get_flagfile_path ( const char* const restrict name );

const char* ishare_get_flagfile_path_shared (
   const char* const restrict name
);

const char* ishare_peek_flagfile_path_shared (void);

#endif  /* _INITRAMFS_RUN_FLAGS_PATH_H_ */
