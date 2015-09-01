/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include "_base.h"
#include "../../common/strutil/compare.h"


void set_splitprog_source_none (
    struct splitprog_source* const sps
) {
    sps->type = SPLITPROG_SOURCE_TYPE_NONE;
}

void set_splitprog_source_filepath (
    struct splitprog_source* const sps,
    const char* const filepath
) {
    sps->type = SPLITPROG_SOURCE_TYPE_FILEPATH;
    (sps->data).filepath = filepath;
}

void set_splitprog_source_pid (
    struct splitprog_source* const sps,
    const pid_t pid
) {
    sps->type = SPLITPROG_SOURCE_TYPE_PID;
    (sps->data).pid = pid;
}

void set_splitprog_source_fd (
    struct splitprog_source* const sps,
    const int fd
) {
    sps->type = SPLITPROG_SOURCE_TYPE_FD;
    (sps->data).fd = fd;
}

void set_splitprog_source_default (
    struct splitprog_source* const sps
) {
    set_splitprog_source_pid ( sps, getppid() );
}

int splitprog_parse_range_arg (
   const char* const restrict input_str,
   size_t* const restrict start_idx_out,
   size_t* const restrict max_idx_out
) {
    /*
     * valid input_str variants, where n and m are integers > 0
     *
     *   (a) n-m        select fields from n to m (inclusive)
     *   (c) n-         select all fields starting from n  (same as n-0)
     *   (d) -m         select all fields up to m          (same as 1-m)
     *   (e) n          select exactly one field           (same as n-n)
     *
     * Note that n == 1 and n == 0 are identical (i.e. start from
     * first field), but m == 1 and m == 0 are not
     * (1: stop at first field, 0: stop at last field).
     * So, in case (e), a 0 means "select all fields" and a 1 means
     * "select first field".
     */
    const char* iter;
    size_t      fields[2];
    unsigned    fieldno;

    *start_idx_out = 0;
    *max_idx_out   = 0;

    if ( STR_IS_EMPTY ( input_str ) ) { return -1; }

    fieldno   = 0;
    fields[0] = 0;
    fields[1] = 0;
    for ( iter = input_str; ; iter++ ) {
        switch ( *iter ) {
            case '\0':
                *start_idx_out = (fields[0] < 1) ? 0 : (fields[0] - 1);
                *max_idx_out   = (fieldno < 1) ? fields[0] : fields[1];
                return 0;

            case '-':
                fieldno++;
                if ( fieldno > 1 ) { return -1; }
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
                /* overflow not checked */
                fields[fieldno] = (10 * fields[fieldno]) + ((*iter) - '0');
                break;

            default:
                /* input_str not valid */
                return -1;
        }
    }

    return -1;
}
