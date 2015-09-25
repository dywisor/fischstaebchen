/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_NET_MAGIC_PACKET_SENDER_H_
#define _COMMON_NET_MAGIC_PACKET_SENDER_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "data_types/magic_packet.h"
#include "data_types/mac_addr.h"


extern const char* const FISCHNET_MAGIC_PACKET_DEFAULT_BROADCAST_ADDR;
/*extern const char* const FISCHNET_MAGIC_PACKET_DEFAULT_SRC_ADDR;*/

int fischnet_send_magic_packet (
    const struct fischnet_magic_packet* const restrict wiz,
    const char* const restrict src_addr,
    const char* const restrict broadcast_addr
);

int fischnet_send_magic_packet_to_mac_addr (
    const struct fischnet_mac_addr* const restrict mac_addr,
    const char* const restrict src_addr,
    const char* const restrict broadcast_addr
);


#endif  /* _COMMON_NET_MAGIC_PACKET_SENDER_H_ */
