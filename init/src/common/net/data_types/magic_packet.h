/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_NET_DATA_TYPES_MAGIC_PACKET_H_
#define _COMMON_NET_DATA_TYPES_MAGIC_PACKET_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "mac_addr.h"

#define FISCHNET_MAGIC_PACKET_BYTE_COUNT  \
    (6 + (16 * FISCHNET_MAC_ADDR_BYTE_COUNT))


struct fischnet_magic_packet {
    unsigned char data [FISCHNET_MAGIC_PACKET_BYTE_COUNT];
};


int fischnet_magic_packet_init_empty ( struct fischnet_magic_packet* const wiz );
int fischnet_magic_packet_init (
          struct fischnet_magic_packet* const restrict wiz,
    const struct fischnet_mac_addr*     const restrict mac_addr
);

void fischnet_magic_packet_free     ( struct fischnet_magic_packet* const wiz );
void fischnet_magic_packet_free_ptr ( struct fischnet_magic_packet** const wiz_ptr );

unsigned const char* fischnet_magic_packet_get_data (
    const struct fischnet_magic_packet* const wiz
);

#endif  /* _COMMON_NET_DATA_TYPES_MAGIC_PACKET_H_ */
