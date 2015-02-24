/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_MOUNT_OPTS_H_
#define _COMMON_FS_MOUNT_OPTS_H_

#include <sys/types.h>

#include "../dynarray.h"

/**
 * Creates a mount opts str from an opts str and int flags.
 * Options from opts_in take precedence.
 *
 * @param flags
 * @param opts_in
 *
 * @return  NULL on error, else opts str
 */
char* get_mount_opts_str (
   const unsigned long flags,
   const char* const opts_in
);

/**
 * Parses a mount opts str and stores mount flags in an int var and
 * any non-flag options in a dynarray.
 *
 * @param opts_in    mount options string (input) (should not be NULL)
 * @param flags_out  mount flags (outvar) (initially set to 0)
 * @param p_darr     option string dynarray (outvar)
 *                    (should be empty, but doesn't have to)
 *
 * @return 0 on success, else non-zero
 */
int parse_mount_opts_str_to_dynarray (
   const char* const opts_in,
   unsigned long* const flags_out,
   struct dynarray* const p_darr
);

/**
 * Similar to parse_mount_opts_str_to_dynarray(),
 * but reassembles the option strings from the dynarray.
 *
 * @param opts_in        mount options string (input) (should not be NULL)
 * @param flags_out      mount flags (outvar) (initially set to 0)
 * @param opts_out_ptr   string pointer for storing the options string
 *
 * @return 0 on success, else non-zero
 */
int parse_mount_opts_str (
   const char* const opts_in,
   unsigned long* const flags_out,
   char** const opts_out_ptr
);

/**
 * Parses a tmpfs size spec, which is an int optionally followed by
 * '%' (size relative to system memory), 'k' (kiB), 'm' (MiB), 'g' (GiB).
 *
 * The resulting size is stored in the size_out parameter, in MiB.
 *
 * @param size_str          size spec str
 * @param system_memory_m   system memory in MiB
 *                           (only used when parsing a percentage)
 * @param size_out          result variable
 *
 * @return 0 on success, -1 on error
 */
int parse_tmpfs_size_spec (
   const char* const size_str, const size_t system_memory_m,
   size_t* const size_out
);

/**
 * Creates a tmpfs size opt str from the given size int (size_t)
 *
 * @param size_m   size in MiB
 *
 * @return NULL on error, else "size=..." str
 */
char* create_tmpfs_size_opt ( const size_t size_m );


#endif /* _COMMON_FS_MOUNT_OPTS_H_ */
