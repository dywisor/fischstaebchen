/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_BASEOPS_H_
#define _COMMON_FS_BASEOPS_H_

#include <sys/types.h>

/**
 * Checks whether the given filesystem path exists (possibly as broken symlink).
 *
 * @param fspath
 *
 * @return 0 if fspath (l)exists, else -1
 */
int check_fs_lexists     ( const char* const fspath );

/** Checks whether the given filesystem path actually exists. */
int check_fs_exists      ( const char* const fspath );

int check_fs_executable  ( const char* const fspath );
int check_fs_readable    ( const char* const fspath );
int check_fs_writable    ( const char* const fspath );
int check_fs_exefile     ( const char* const fspath );
int check_dir_exists     ( const char* const fspath );
int dodir_mode           ( const char* const fspath, mode_t mode );
int dodir                ( const char* const fspath );
int makedirs_mode        ( const char* const fspath, mode_t parent_mode, mode_t mode );
int makedirs             ( const char* const fspath );
int makedirs_parents     ( const char* const fspath, mode_t parent_mode );
int dosym                ( const char* const target, const char* const link_name );
int unlink_if_exists     ( const char* const fspath );
int is_mounted           ( const char* const fspath );

#endif /* _COMMON_FS_BASEOPS_H_ */
