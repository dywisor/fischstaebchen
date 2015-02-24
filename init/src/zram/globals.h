/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _ZRAM_GLOBALS_H_
#define _ZRAM_GLOBALS_H_

#include <stdint.h>
#include <sys/types.h>

#include "config.h"


#define ZRAM_BYTES_TO_MBYTES_FACTOR   1048576
#define _ZRAM_BASE_NAME               "zram"
#define _ZRAM_BASE_PATH               GET_SYSFS_PATH("/block/" _ZRAM_BASE_NAME)
#define _ZRAM_BASE_DEVPATH            GET_DEVFS_PATH("/" _ZRAM_BASE_NAME)


struct zram_globals_type {
   uint8_t     next_free_identifier;
   uint64_t    system_page_size;
   const char* base_name;
   const char* base_path;
};

extern struct zram_globals_type* zram_globals;

int  zram_init_globals (void);
void zram_free_globals (void);

char* get_zram_ident_str ( const uint8_t identifier );
char* get_zram_name      ( const char* const ident_str );
char* get_zram_path      ( const char* const ident_str );
char* get_zram_devpath   ( const char* const ident_str );

int get_zram_name_path_triple (
   const uint8_t identifier, char** const data_out
);

void zram_export_next_free_identifier ( const char* const varname );
uint8_t zram_import_next_free_identifier ( const char* const varname );

#endif
