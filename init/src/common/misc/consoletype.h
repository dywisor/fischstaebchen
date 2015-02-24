/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MISC_CONSOLETYPE_H_
#define _COMMON_MISC_CONSOLETYPE_H_

#include <sys/stat.h>
#include <sys/types.h>

enum {
   CONSOLE_TYPE_UNDEF,
   CONSOLE_TYPE_SERIAL,
   CONSOLE_TYPE_VT,
   CONSOLE_TYPE_PTY
};

int         get_fd_consoletype  ( const int fd );
int         get_consoletype     ( const char* const path );
const char* get_consoletype_str ( const int consoletype );
void        export_consoletype  ( const int );

#endif /* _COMMON_MISC_CONSOLETYPE_H_ */
