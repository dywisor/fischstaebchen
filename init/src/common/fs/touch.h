/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_TOUCH_H_
#define _COMMON_FS_TOUCH_H_

#include <sys/types.h>

/**
 * Creates an empty file if it doesn't exist yet.
 *
 * @param filepath
 * @param filemode
 *
 * @return 0 on success, else non-zero
 */
int pseudo_touch_file (
   const char* const filepath, const mode_t filemode
);

/**
 * Like pseudo_touch_file(), but creates all necessary directories.
 */
int pseudo_touch_file_makedirs (
   const char* const filepath, const mode_t filemode
);

#endif /* _COMMON_FS_TOUCH_H_ */
