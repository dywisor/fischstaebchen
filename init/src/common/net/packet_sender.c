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
#include <errno.h>
#include <string.h>

#include "packet_sender.h"
#include "../message.h"
#include "../mac.h"


int fischnet_send_data_packet (
    const int            sock_type,
    const in_addr_t      src_addr,
    const uint16_t       src_port,
    const in_addr_t      dst_addr,
    const uint16_t       dst_port,
    const int            sender_flags,
    const void* const    data,
    const size_t         data_len
) {
    static const int CONST_ITRUE = 1;

    int sock_fd;
    struct sockaddr_in sock_src_addr;
    struct sockaddr_in sock_dst_addr;

    sock_src_addr.sin_family      = AF_INET;
    sock_src_addr.sin_addr.s_addr = src_addr;
    sock_src_addr.sin_port        = htons ( src_port );

    sock_dst_addr.sin_family      = AF_INET;
    sock_dst_addr.sin_addr.s_addr = dst_addr;
    sock_dst_addr.sin_port        = htons ( dst_port );

    sock_fd = socket ( AF_INET, sock_type, 0 );
    if ( sock_fd < 0 ) { return -1; }

    if ( sender_flags & FISCHNET_SEND_DATA_PACKET_WANT_BROADCAST ) {
        if (
            setsockopt (
                sock_fd, SOL_SOCKET,
                SO_BROADCAST, &CONST_ITRUE, sizeof CONST_ITRUE
            ) < 0
        ) {
            X_PRESERVE_ERRNO (
                printf_debug (
                    NULL,
                    "Failed to set broadcast socket option: %s", "\n",
                    strerror(esav)
                )
            );
            close ( sock_fd );
            return -1;
        }
    }

    if (
        bind (
            sock_fd,
            (struct sockaddr*)(&sock_src_addr), sizeof sock_src_addr
        ) < 0
    ) {
        X_PRESERVE_ERRNO (
            printf_debug (
                NULL,
                "Failed to bind socket: %s", "\n",
                strerror(esav)
            )
        );
        close ( sock_fd );
        return -1;
    }

    if (
        sendto (
            sock_fd,
            data, data_len,
            0,
            (struct sockaddr*)(&sock_dst_addr), sizeof sock_dst_addr
        ) < 0
    ) {
        X_PRESERVE_ERRNO (
            printf_debug (
                NULL,
                "Failed to send data: %s", "\n",
                strerror(esav)
            )
        );
        close ( sock_fd );
        return 1;
    }

    close ( sock_fd );
    return 0;
}
