/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_NET_PACKET_SENDER_H_
#define _COMMON_NET_PACKET_SENDER_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum {
    FISCHNET_SEND_DATA_PACKET__NONE = 0,

    FISCHNET_SEND_DATA_PACKET_WANT_BROADCAST = 0x1
};

int fischnet_send_data_packet (
    const int            sock_type,
    const in_addr_t      src_addr,
    const uint16_t       src_port,
    const in_addr_t      dst_addr,
    const uint16_t       dst_port,
    const int            sender_flags,
    const void* const    data,
    const size_t         data_len
);


#endif  /* _COMMON_NET_PACKET_SENDER_H_ */
