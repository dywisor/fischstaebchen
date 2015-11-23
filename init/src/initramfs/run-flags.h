/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _INITRAMFS_RUN_FLAGS_H_
#define _INITRAMFS_RUN_FLAGS_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "run-flags-path.h"
#include "../common/mac.h"
#include "../common/decision_bool.h"


/*
#define __ISHARE_FLAG_PATH(_unused, _arg, ...)  \
   ( \
      (_arg == NULL) \
      ? ishare_peek_flagfile_path_shared() \
      : ishare_get_flagfile_path_shared(_arg) \
   )

#define ISHARE_FLAG_PATH(...)  \
   __ISHARE_FLAG_PATH(NULL, ##__VA_ARGS__, NULL)
*/


extern char* ishare_get_flagfile_path ( const char* const restrict name );

extern const char* ishare_get_flagfile_path_shared (
   const char* const restrict name
);

extern const char* ishare_peek_flagfile_path_shared (void);


/* extension: */
DBOOL_TYPE ishare_check_flag        ( const char* const restrict name );

int        ishare_has_flag          ( const char* const restrict name );
int        ishare_has_not_flag      ( const char* const restrict name );
int        ishare_add_flag          ( const char* const restrict name );
int        _ishare_del_flag         ( const char* const restrict name );
#define    ishare_del_flag(n)       IGNORE_RETCODE ( _ishare_del_flag(n) )
int        ishare_set_flag          (
   const char* const restrict name, const int bval
);
int        ishare_set_flag_from_str (
   const char* const restrict name,
   const char* const restrict str_value
);
int        ishare_set_flag_from_char (
   const char* const restrict name,
   const char chr_value
);

#endif  /* _INITRAMFS_RUN_FLAGS_H_ */
