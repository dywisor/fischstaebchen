/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_LOOPDEV_
#define _COMMON_FS_LOOPDEV_

#include <stdint.h>

/**
 * Sets up a loop device and attaches a file to it.
 *
 *
 * @param filepath      loop fs (usually a filepath)
 * @param file_offset   loop fs offset (usually 0)
 * @param want_ro       int that indicates whether filepath should be
 *                      opened in readonly (1) or read-write (0) mode
 * @param loopdev_out   str pointer for storing the path to the created
 *                      loop device
 *
 * @return 0 on success, else non-zero
 */
int setup_loop_dev (
   const char* const filepath,
   const uint64_t file_offset,
   const int want_ro,
   char** const loopdev_out
);

#endif /* _COMMON_FS_LOOPDEV_ */
