/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "magic_packet.h"
#include "mac_addr.h"
#include "../../mac.h"


static void _fischnet_magic_packet_zap (
    struct fischnet_magic_packet* const wiz
);

static int _fischnet_magic_packet_assign_from_mac_addr (
          struct fischnet_magic_packet* const restrict wiz,
    const struct fischnet_mac_addr*     const restrict mac_addr
);


int fischnet_magic_packet_init_empty (
    struct fischnet_magic_packet* const wiz
) {
    _fischnet_magic_packet_zap ( wiz );
    return 0;
}

int fischnet_magic_packet_init (
          struct fischnet_magic_packet* const restrict wiz,
    const struct fischnet_mac_addr*     const restrict mac_addr
) {
    return (
        (fischnet_magic_packet_init_empty ( wiz ) != 0) \
            ? -1 \
            : _fischnet_magic_packet_assign_from_mac_addr ( wiz, mac_addr )
    );
}

void fischnet_magic_packet_free ( struct fischnet_magic_packet* const UNUSED(wiz) ) {
    /*if ( wiz == NULL ) { return; }*/
    return;
}

void fischnet_magic_packet_free_ptr (
    struct fischnet_magic_packet** const wiz_ptr
) {
    if ( wiz_ptr == NULL ) { return; }
    fischnet_magic_packet_free ( *wiz_ptr );
    x_free ( *wiz_ptr );
}

unsigned const char* fischnet_magic_packet_get_data (
    const struct fischnet_magic_packet* const wiz
) {
    return (wiz == NULL) ? NULL : (wiz->data);
}


static void _fischnet_magic_packet_zap (
    struct fischnet_magic_packet* const wiz
) {
    size_t k;

    k = 0;
    for ( ; k < 6; k++ ) { (wiz->data) [k] = 0xff; }
    for ( ; k < FISCHNET_MAGIC_PACKET_BYTE_COUNT; k++ ) { (wiz->data) [k] = 0; }
}


static int _fischnet_magic_packet_assign_from_mac_addr (
          struct fischnet_magic_packet* const restrict wiz,
    const struct fischnet_mac_addr*     const restrict mac_addr
) {
    size_t k;

    /*
     * "repeat mac addr 16 times, starting at offset 6" (7th byte)
     *
     * or: fill up the magic packet up to its max size with data from
     *     the mac address where each position <k> in the magic packet
     *     is mapped to "<k>-<offset> % <mac addr byte count>"
     */
    for ( k = 6; k < FISCHNET_MAGIC_PACKET_BYTE_COUNT; k++ ) {
        (wiz->data) [k] = (
            (mac_addr->addr) [(k -6) % FISCHNET_MAC_ADDR_BYTE_COUNT]
        );
    }

    return 0;
}
