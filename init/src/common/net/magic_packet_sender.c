/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "packet_sender.h"
#include "magic_packet_sender.h"


const char* const \
    FISCHNET_MAGIC_PACKET_DEFAULT_BROADCAST_ADDR = "255.255.255.0";


static int _fischnet_send_magic_packet_from_to (
    const struct fischnet_magic_packet* const restrict wiz,
    const in_addr_t src_addr, const uint16_t src_port,
    const in_addr_t dst_addr, const uint16_t dst_port
);


int fischnet_send_magic_packet (
    const struct fischnet_magic_packet* const restrict wiz,
    const char* const restrict src_addr,
    const char* const restrict broadcast_addr
) {
    static const uint16_t dst_ports [] = { 7, 9, 0 };

    const in_addr_t dst_addr_in = inet_addr (
        (broadcast_addr != NULL) \
            ? broadcast_addr \
            : FISCHNET_MAGIC_PACKET_DEFAULT_BROADCAST_ADDR
    );
    const in_addr_t src_addr_in = (
        (src_addr != NULL) ? inet_addr ( src_addr ) : INADDR_ANY
    );

    const uint16_t* dst_port_iter;
    int ret;

    ret = 0;
    for ( dst_port_iter = dst_ports; *dst_port_iter != 0; dst_port_iter++ ) {
        const int sret = _fischnet_send_magic_packet_from_to (
            wiz,
            src_addr_in, 0,
            dst_addr_in, *dst_port_iter
        );

        if ( (sret != 0) && (ret >= 0) ) { ret = sret; }
    }

    return ret;
}

int fischnet_send_magic_packet_to_mac_addr (
    const struct fischnet_mac_addr* const restrict mac_addr,
    const char* const restrict src_addr,
    const char* const restrict broadcast_addr
) {
    struct fischnet_magic_packet wiz;
    int ret;

    if ( fischnet_magic_packet_init ( &wiz, mac_addr ) != 0 ) {
        ret = -1;
    } else {
        ret = fischnet_send_magic_packet ( &wiz, src_addr, broadcast_addr );
    }

    fischnet_magic_packet_free ( &wiz );
    return ret;
}


static int _fischnet_send_magic_packet_from_to (
    const struct fischnet_magic_packet* const restrict wiz,
    const in_addr_t src_addr, const uint16_t src_port,
    const in_addr_t dst_addr, const uint16_t dst_port
) {
    return fischnet_send_data_packet (
        SOCK_DGRAM,
        src_addr, src_port,
        dst_addr, dst_port,
        FISCHNET_SEND_DATA_PACKET_WANT_BROADCAST,
        (wiz->data), FISCHNET_MAGIC_PACKET_BYTE_COUNT
    );
}
