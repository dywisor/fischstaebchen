/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_FILEIO_H_
#define _COMMON_FS_FILEIO_H_

#include <stdint.h>
#include <inttypes.h>

#include "../data_types/dynarray.h"

#ifndef FILEIO_READFILE_BUFSIZE
enum { FILEIO_READFILE_BUFSIZE = 80 };
#endif

/**
 * Reads a cmdline file.
 *
 * @UNDOCUMENTED
 *
 */
int read_cmdline_file (
   const char* const filepath,
   char** const str_out,
   struct dynarray* const p_darr
);

/**
 * Reads a sysfs file and stores its content in a dynamically allocated
 * str, accessible via the given str_out pointer.
 *
 * @param filepath   path to the file that should be read
 * @param str_out    pointer to a str (should point to NULL)
 *
 * @return 0 on success, else non-zero
 */
int read_sysfs_file (
   const char* const filepath,
   char** const str_out
);

int sysfs_read_str (
   const char* const dirpath,
   const char* const filename,
   char** const str_out
);

/**
 * Writes text to a file.
 *
 * @param filepath   path to the file that should be written
 * @param text       text to be written
 * @param flags      additional flags for open(), e.g. O_TRUNC or O_APPEND
 * @param mode       mode for creating the file (requires O_CREAT in flags)
 *
 * @return 0 on success, else non-zero
 */
int write_text_file (
   const char* const filepath,
   const char* const text,
   const int    flags,
   const mode_t mode
);


/**
 * Writes text to a sysfs file.
 *
 * @param filepath  path to the file that should be written
 * @param text      text to be written
 *
 * @return 0 on success, else non-zero
 */
int write_sysfs_file (
   const char* const filepath,
   const char* const text
);

/**
 * write_sysfs_file() wrapper that concatenes a dirpath,filename path.
 *
 * @param dirpath     directory part of the filepath (must not be NULL)
 * @param filepath    filename (must not be NULL)
 * @param str         str to write (may be NULL)
 *
 * @return 0 on success, else non-zero
 */
int sysfs_write_str (
   const char* const dirpath,
   const char* const filename,
   const char* const str
);

/**
 * sysfs_write_str() wrapper that writes an int64_t.
 */
int sysfs_write_int64_decimal (
   const char* const dirname,
   const char* const filename,
   const int64_t value
);

/**
 * sysfs_write_str() wrapper that writes an uint64_t.
 */
int sysfs_write_uint64_decimal (
   const char* const dirpath,
   const char* const filename,
   const uint64_t value
);


#endif /* _COMMON_FS_FILEIO_H_ */
