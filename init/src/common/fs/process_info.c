/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>


#include "process_info.h"
#include "../config.h"
#include "../mac.h"
#include "../data_types/dynarray.h"
#include "../data_types/str_dynarray.h"
#include "../data_types/dynstr.h"
#include "../strutil/format.h"


static void zap_environ_varname_value_pair (
   struct environ_varname_value_pair* const evp
) {
   evp->_buf       = NULL;
   evp->_split_pos = NULL;
   evp->varname    = NULL;
   evp->value      = NULL;
}

static int _environ_varname_value_pair_set_no_reinit (
    struct environ_varname_value_pair* const evp,
    const char* const varname_value_str
);


void environ_varname_value_pair_free (
    struct environ_varname_value_pair* const evp
) {
    if ( evp == NULL ) { return; }
    x_free ( evp->_buf );
    zap_environ_varname_value_pair ( evp );
}

void environ_varname_value_pair_free_ptr (
    struct environ_varname_value_pair** const p_evp
) {
    if ( p_evp == NULL ) { return; }
    environ_varname_value_pair_free ( *p_evp );
    x_free ( *p_evp );
}

int environ_varname_value_pair_init (
    struct environ_varname_value_pair* const evp,
    const char* const varname_value_str
) {
    if ( evp == NULL ) { return -1; }
    zap_environ_varname_value_pair ( evp );
    return (
        (varname_value_str == NULL) ? 0 : \
            _environ_varname_value_pair_set_no_reinit (
                evp, varname_value_str
            )
    );
}

int environ_varname_value_pair_set (
    struct environ_varname_value_pair* const evp,
    const char* const varname_value_str
) {
    if ( evp == NULL ) { return -1; }
    environ_varname_value_pair_free ( evp );
    return (
        (varname_value_str == NULL) ? 0 : \
            _environ_varname_value_pair_set_no_reinit (
                evp, varname_value_str
            )
    );
}

int split_environ_varname_value_str (
    const char* const restrict varname_value_str,
    char** const restrict      varname_out,
    char** const restrict      value_out
) {
    struct environ_varname_value_pair evp;
    int ret;

    /* safety checks */
    if ( varname_out == NULL )        { return -1; }
    *varname_out = NULL;

    if ( value_out == NULL )          { return -1; }
    *value_out = NULL;

    if ( varname_value_str == NULL )  { return -1; }

    ret = environ_varname_value_pair_init ( &evp, varname_value_str );

    /* copy varnameto *varname_out */
    switch ( ret ) {
        case -1: break;

        case ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS:
        case ENVIRON_VARNAME_VALUE_PAIR_PARSE_NOVALUE:
            *varname_out = strdup ( evp.varname );
            if ( *varname_out == NULL ) { ret = -1; }
            break;
    }

    /* copy value to *value_out */
    switch ( ret ) {
        case -1: break;

        case ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS:
            *value_out = strdup ( evp.value );
            if ( *value_out == NULL ) { ret = -1; }
            break;
    }

    environ_varname_value_pair_free ( &evp );
    return ret;
}

static int _environ_varname_value_pair_set_no_reinit (
    struct environ_varname_value_pair* const evp,
    const char* const varname_value_str
) {
   static const char* const EMPTY_STR = "";

   /* assert evp non-NULL and initialized */
   /* assert varname_value_str non-NULL */

   /* need a private copy, the first '=' will be replaced with a '\0' */
   evp->_buf = strdup ( varname_value_str );
   if ( evp->_buf == NULL ) { return -1; }

   evp->varname    = evp->_buf;
   evp->_split_pos = strchr ( evp->_buf, '=' );

   if ( evp->_split_pos == NULL ) {
      /* input str w/o a '=' char */
      evp->value = EMPTY_STR;
      return ENVIRON_VARNAME_VALUE_PAIR_PARSE_NOVALUE;

   } else {
      /* found '=', replace it with '\0', set value str begin to split_pos+1 */
      evp->value = (evp->_split_pos + 1);
      *(evp->_split_pos) = '\0';
      return ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS;
   }
}

void environ_varname_value_pair_set_dynarray_destructor (
    struct dynarray* const restrict p_darr
) {
    if ( p_darr != NULL ) {
        p_darr->item_destructor = \
            (dynarray_free_item_ptr_func)environ_varname_value_pair_free_ptr;
    }
}

int environ_varname_value_pair_convert_array (
    const size_t argc,
    const char* const* const restrict argv,
    struct dynarray* const restrict evp_arr_out
) {
    struct environ_varname_value_pair* evp;
    size_t k;
    int    ret;
    int    parse_ret;

    /* store "most significant" return code in ret,
     * where any retcode != 0 is more significant than a value of 0
     */
    ret = ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS;
    for ( k = 0; k < argc; k++ ) {
        /* need a new data object */
        evp = malloc ( sizeof *evp );
        if ( evp == NULL ) { return -1; }

        parse_ret = environ_varname_value_pair_init ( evp, argv [k] );
        if ( ret == ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS ) {
            ret = parse_ret;
        }

        switch ( parse_ret ) {
            case ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS:
            case ENVIRON_VARNAME_VALUE_PAIR_PARSE_NOVALUE:
                if ( dynarray_append ( evp_arr_out, evp ) != 0 ) {
                    environ_varname_value_pair_free_ptr ( &evp );
                    return -1;
                }
                break;

            default:
                /* parse_ret indicates a memory/... error (<0)
                 * or an unknown error (>0 != NOVALUE), return immediately
                 */

                /* evp not added to dynarray, free it */
                environ_varname_value_pair_free_ptr ( &evp );
                return parse_ret;
        }

        /* evp added to dynarray, reset the pointer */
        evp = NULL;
    }

    return ret;
}



static int _proc_read_environ_or_cmdline_from_fd (
    const int fd,
    struct dynarray* const p_darr
) {
    struct dynstr_data dynstr;
    X_DECLARE_FILE_READ_BUFFER(buf);
    ssize_t nbuf;
    ssize_t iter;

    if ( p_darr == NULL ) { return -1; }

    if ( dynstr_data_init ( &dynstr, 0 ) != 0 ) { return -1; }

    while ( X_FILE_READ_BUFFER_NBUF(fd, buf, nbuf) > 0 ) {
        for ( iter = 0; iter < nbuf; iter++ ) {
            switch ( buf [iter] ) {
                case '\0':
                    /*
                     * end of arg, move dynstr char* to dynarray, reset dynstr
                     */
                    if (
                        dynarray_append_dynstr_unref ( p_darr, &dynstr ) != 0
                    ) {
                        /* dynstr freed by dynarray_append_dynstr_unref() */
                        return -1;
                    }
                    break;

                default:
                    /* append char to current arg */
                    if (
                        dynstr_data_append_char ( &dynstr, buf [iter] ) != 0
                    ) {
                        /* dynstr freed by dynstr_data_append_char() */
                        return -1;
                    }
                    break;  /* nop */
            }
        }
    }

    /* read error? */
    if ( nbuf < 0 ) {
        dynstr_data_free ( &dynstr );
        return -1;
    }

    /* possibly (implicit) end of arg,
     *  FIXME not sure how this could occur given that '\0' is catched
     *  in the while-read loop above and read errors can't reach the
     *  the following code
     */
    if ( dynstr_data_get(&dynstr) != NULL ) {
        if ( dynarray_append_dynstr_unref ( p_darr, &dynstr ) != 0 ) {
            /* dynstr freed by dynarray_append_dynstr_unref() */
            return -1;
        }
    }

    return 0;
}

int proc_read_cmdline_from_fd (
    const int fd,
    struct dynarray* const p_darr
) {
    return _proc_read_environ_or_cmdline_from_fd ( fd, p_darr );
}

int proc_read_cmdline_from_file (
    const char* const cmdline_filepath,
    struct dynarray* const p_darr
) {
    int ret;

    /*
     * The X_WITH_OPEN_FILE_CALL_FUNC macro
     * does the following (+/- error checking)::
     *
     *   fd  = open (...);
     *   ret = proc_read_cmdline_from_fd (fd, ...);
     *   close ( fd );
     *
     */
    X_WITH_OPEN_FILE_CALL_FUNC (
        ret, cmdline_filepath, O_RDONLY,
        proc_read_cmdline_from_fd ( fd, p_darr )
    );

    return ret;
}

int proc_read_cmdline_from_pid (
    const pid_t pid,
    struct dynarray* const p_darr
) {
#define BUFSIZE  PATH_MAX
    char cmdline_filepath [BUFSIZE];

    if (
        str_format_check_success (
            cmdline_filepath, BUFSIZE,
            "%s/%ld/%s", PROC_PATH, (long)(pid), "cmdline"
        )
    ) {
        return proc_read_cmdline_from_file ( cmdline_filepath, p_darr );
    } else {
        return -1;
    }

#undef  BUFSIZE
}


int proc_read_environ_from_fd (
    const int fd,
    struct dynarray* const p_darr
) {
    return _proc_read_environ_or_cmdline_from_fd ( fd, p_darr );
}

int proc_read_environ_from_file (
   const char* const environ_filepath,
   struct dynarray* const p_darr
) {
    int ret;

    X_WITH_OPEN_FILE_CALL_FUNC (
        ret, environ_filepath, O_RDONLY,
        proc_read_environ_from_fd ( fd, p_darr )
    );

    return ret;
}

int proc_read_environ_from_pid (
    const pid_t pid,
    struct dynarray* const p_darr
) {
#define BUFSIZE  PATH_MAX
    char environ_filepath [BUFSIZE];

    if (
        str_format_check_success (
            environ_filepath, BUFSIZE,
            "%s/%ld/%s", PROC_PATH, (long)(pid), "environ"
        )
    ) {
        return proc_read_environ_from_file ( environ_filepath, p_darr );
    } else {
        return -1;
    }

#undef  BUFSIZE
}



