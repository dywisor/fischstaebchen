/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MISC_ENV_H_
#define _COMMON_MISC_ENV_H_

int export_env ( const char* const varname, const char* const value );
#define env_export(...)  export_env(__VA_ARGS__)

int export_env_if_not_set (
   const char* const varname, const char* const value
);
#define env_export_if_not_set(...) export_env_if_not_set(__VA_ARGS__)

int append_to_path ( const char* const dirpath );
int add_to_path    ( const char* const dirpath );

int export_env_shbool ( const char* const varname, const int boolval );
#define env_export_shbool(...)  export_env_shbool(__VA_ARGS__)

#endif /* _COMMON_MISC_ENV_H_ */
