/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_NET_DATA_TYPES_MAC_ADDR_H_
#define _COMMON_NET_DATA_TYPES_MAC_ADDR_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define FISCHNET_MAC_ADDR_BYTE_COUNT  6

enum {
    FISCHNET_MAC_ADDR_PARSE_RESULT_SUCCESS = 0,
    FISCHNET_MAC_ADDR_PARSE_RESULT_TOO_FEW_BLOCKS,
    FISCHNET_MAC_ADDR_PARSE_RESULT_TOO_MANY_BLOCKS,
    FISCHNET_MAC_ADDR_PARSE_RESULT_EMPTY_BLOCK,
    FISCHNET_MAC_ADDR_PARSE_RESULT_INVALID_TOKEN,

    FISCHNET_MAC_ADDR_PARSE_RESULT__STATE_COUNT
};

extern const char* FISCHNET_MAC_ADDR_PARSE_RESULT_STR [FISCHNET_MAC_ADDR_PARSE_RESULT__STATE_COUNT];


struct fischnet_mac_addr {
    unsigned char addr [FISCHNET_MAC_ADDR_BYTE_COUNT];
    char*         addr_str;
};


int fischnet_mac_addr_init_empty ( struct fischnet_mac_addr* const mac_addr );
int fischnet_mac_addr_init_str (
    struct fischnet_mac_addr* const mac_addr,
    const char* const mac_addr_str
);

void fischnet_mac_addr_free     ( struct fischnet_mac_addr* const mac_addr );
void fischnet_mac_addr_free_ptr ( struct fischnet_mac_addr** const mac_addr_ptr );
const char* fischnet_mac_addr_get_str ( struct fischnet_mac_addr* const mac_addr );

const char* fischnet_mac_addr_strerror ( const int parse_result );

#endif  /* _COMMON_NET_DATA_TYPES_MAC_ADDR_H_ */
