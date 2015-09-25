/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "mac_addr.h"
#include "../../mac.h"



const char* \
    FISCHNET_MAC_ADDR_PARSE_RESULT_STR \
    [FISCHNET_MAC_ADDR_PARSE_RESULT__STATE_COUNT] = {
        [FISCHNET_MAC_ADDR_PARSE_RESULT_SUCCESS]         = "success",
        [FISCHNET_MAC_ADDR_PARSE_RESULT_TOO_FEW_BLOCKS]  = "too few blocks",
        [FISCHNET_MAC_ADDR_PARSE_RESULT_TOO_MANY_BLOCKS] = "too many blocks",
        [FISCHNET_MAC_ADDR_PARSE_RESULT_EMPTY_BLOCK]     = "empty block",
        [FISCHNET_MAC_ADDR_PARSE_RESULT_INVALID_TOKEN]   = "invalid token"
};



static void _fischnet_mac_addr_zap (
    struct fischnet_mac_addr* const mac_addr
);

static char* _fischnet_mac_addr_create_str (
    const struct fischnet_mac_addr* const mac_addr
);

static int _fischnet_mac_addr_parse_addr_str (
    struct fischnet_mac_addr* const mac_addr,
    const char* const mac_addr_str
);


const char* fischnet_mac_addr_strerror ( const int parse_result ) {
    if ( parse_result < 0 ) {
        return "error";
    } else if (
        parse_result >= FISCHNET_MAC_ADDR_PARSE_RESULT__STATE_COUNT
    ) {
        return "unknown";
    } else {
        return FISCHNET_MAC_ADDR_PARSE_RESULT_STR [parse_result];
    }
}


int fischnet_mac_addr_init_empty (
    struct fischnet_mac_addr* const mac_addr
) {
    _fischnet_mac_addr_zap ( mac_addr );
    return 0;
}

int fischnet_mac_addr_init_str (
    struct fischnet_mac_addr* const mac_addr,
    const char* const mac_addr_str
) {
    return (
        (fischnet_mac_addr_init_empty ( mac_addr ) != 0) \
            ? -1 \
            : _fischnet_mac_addr_parse_addr_str ( mac_addr, mac_addr_str )
    );
}

void fischnet_mac_addr_free ( struct fischnet_mac_addr* const mac_addr ) {
    if ( mac_addr == NULL ) { return; }
    x_free ( mac_addr->addr_str );
    _fischnet_mac_addr_zap ( mac_addr );
}

void fischnet_mac_addr_free_ptr (
    struct fischnet_mac_addr** const mac_addr_ptr
) {
    if ( mac_addr_ptr == NULL ) { return; }
    fischnet_mac_addr_free ( *mac_addr_ptr );
    x_free ( *mac_addr_ptr );
}

const char* fischnet_mac_addr_get_str (
    struct fischnet_mac_addr* const mac_addr
) {
    if ( mac_addr->addr_str == NULL ) {
        mac_addr->addr_str = _fischnet_mac_addr_create_str ( mac_addr );
    }
    return mac_addr->addr_str;
}


static void _fischnet_mac_addr_zap (
    struct fischnet_mac_addr* const mac_addr
) {
    size_t k;

    for ( k = 0; k < FISCHNET_MAC_ADDR_BYTE_COUNT; k++ ) {
        (mac_addr->addr) [k] = 0;
    }

    mac_addr->addr_str = NULL;
}


static int _fischnet_mac_addr_create_str__conv (
    char** const str_pos,
    unsigned int val
) {
    switch ( val ) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
            **str_pos = '0' + val;
            (*str_pos)++;
            return 0;

        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
            **str_pos = 'A' + 0xa + val;
            (*str_pos)++;
            return 0;

        default:
            return -1;
    }
}

static char* _fischnet_mac_addr_create_str (
    const struct fischnet_mac_addr* const mac_addr
) {
    /* 2 chars per byte (2*n bytes)
     * (n-1) for separating fields
     * + terminating '\0'
     */
    static const size_t FISCHNET_MAC_ADDR_STR_SIZE = (
        ( 2 * FISCHNET_MAC_ADDR_BYTE_COUNT )
        + ( FISCHNET_MAC_ADDR_BYTE_COUNT - 1 )
        + 1
    );

    char*  str;
    char*  str_pos;
    size_t k;
    div_t  div_res;

    RET_NULL_IFNOT_MALLOC ( str, FISCHNET_MAC_ADDR_STR_SIZE );
    str_pos = str;

    for ( k = 0; k < FISCHNET_MAC_ADDR_BYTE_COUNT; k++ ) {
        div_res = div ( (mac_addr->addr) [k], 16 );

        if ( str_pos != str ) { *(str_pos++) = '-'; }

        if (
               (_fischnet_mac_addr_create_str__conv ( &str_pos, div_res.quot ) != 0)
            || (_fischnet_mac_addr_create_str__conv ( &str_pos, div_res.rem  ) != 0)
        ) {
            x_free ( str );
            return NULL;
        }
    }

    return str;
}


static int _fischnet_mac_addr_parse_addr_str (
    struct fischnet_mac_addr* const mac_addr,
    const char* const mac_addr_str
) {
    const char* input_str_pos;
    size_t byte_pos;
    unsigned block_is_empty;

    /* expects zero-initialized mac_addr */
    /* _fischnet_mac_addr_zap ( mac_addr ); */

    if ( (mac_addr_str == NULL) || (*mac_addr_str == '\0') ) { return -1; }

    byte_pos       = 0;
    block_is_empty = 1;
    input_str_pos  = mac_addr_str;

    while ( byte_pos < FISCHNET_MAC_ADDR_BYTE_COUNT ) {
        switch ( *input_str_pos ) {
            case '\0':
                if ( block_is_empty ) {
                    return FISCHNET_MAC_ADDR_PARSE_RESULT_EMPTY_BLOCK;

                } else if ( byte_pos == (FISCHNET_MAC_ADDR_BYTE_COUNT - 1) ) {
                    return FISCHNET_MAC_ADDR_PARSE_RESULT_SUCCESS;

                } else {
                    return FISCHNET_MAC_ADDR_PARSE_RESULT_TOO_FEW_BLOCKS;
                }

                return -1; /* unreachable */

            case '-':
            case ':':
            case '_':
                /* field separator */
                /*  idea: store the first field sep in (char field_sep)
                 *  and restrict subsequent separators to this char
                 */
                if ( block_is_empty ) {
                    return FISCHNET_MAC_ADDR_PARSE_RESULT_EMPTY_BLOCK;
                }
                block_is_empty = 1;
                byte_pos++;
                input_str_pos++;
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                block_is_empty = 0;
                (mac_addr->addr) [byte_pos] = (
                    (16 * ((mac_addr->addr) [byte_pos]))
                    + ((*input_str_pos) - '0')
                );
                input_str_pos++;
                break;

            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                block_is_empty = 0;
                (mac_addr->addr) [byte_pos] = (
                    (16 * ((mac_addr->addr) [byte_pos]))
                    + ((*input_str_pos) - 'a' + 0xa)
                );
                input_str_pos++;
                break;

            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                block_is_empty = 0;
                (mac_addr->addr) [byte_pos] = (
                    (16 * ((mac_addr->addr) [byte_pos]))
                    + ((*input_str_pos) - 'A' + 0xA)
                );
                input_str_pos++;
                break;

            default:
                return FISCHNET_MAC_ADDR_PARSE_RESULT_INVALID_TOKEN;
        }

    }

    return FISCHNET_MAC_ADDR_PARSE_RESULT_TOO_MANY_BLOCKS;
}
