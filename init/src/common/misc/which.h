/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MISC_WHICH_H_
#define _COMMON_MISC_WHICH_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

enum {
    WHICH_STATUS_FOUND_PROG = 0,
    WHICH_STATUS_DID_NOT_FIND_PROG,
    WHICH_STATUS_BAD_USAGE
};

char* which  ( const char* const prog_name );
int   qwhich ( const char* const prog_name );

char* whichmore (
    const char* const prog_name,
    const char* const search_path,
    const char* const search_pathext,
    const char* const root
);

int qwhichmore (
    const char* const prog_name,
    const char* const search_path,
    const char* const search_pathext,
    const char* const root
);


#endif  /* _COMMON_MISC_WHICH_H_ */
