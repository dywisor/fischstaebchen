/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_FSPATH_H_
#define _COMMON_FS_FSPATH_H_

typedef int (*fspath_callback_function) (
   const char* const fspath,
   const char* const basename,
   void* const p_data
);

struct fspath_info {
   char* _path;
   const char* basename;
   const char* dirname;
};

__attribute__((warn_unused_result))
char* fspath_strip ( const char* const path );

void fspath_info_init ( struct fspath_info* const p_info, char* const path );
void fspath_info_free ( struct fspath_info* const p_info );

int fspath_downwards_do (
   const char* const path,
   fspath_callback_function parent_callback,
   void* parent_callback_data,
   fspath_callback_function fspath_callback,
   void* fspath_callback_data
);

int fspath_get_dirname_basename (
   struct fspath_info* const p_info, const char* const path
);


#endif /* _COMMON_FS_FSPATH_H_ */
