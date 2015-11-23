/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "which.h"
#include "../mac.h"
#include "../strutil/compare.h"
#include "../strutil/split.h"
#include "../data_types/dynstr.h"
#include "../fs/baseops.h"


static int _locate_progam (
    const char* const restrict search_path_sep,
    const char* const restrict pathext_sep,
    const char* const restrict search_path,
    const char* const restrict pathext,
    const char* const restrict root,
    const char* const restrict prog_name,
    char** const restrict prog_path_out
);


static int _any_which (
    const char* const prog_name, char** const prog_path_out
) {
    if ( STR_IS_EMPTY(prog_name) ) { return 1; }

    return _locate_progam (
        NULL, NULL,
        getenv ( "PATH" ),
        DEFAULT_SEARCH_PATHEXT,
        NULL, prog_name, prog_path_out
    );
}

static int _any_whichmore (
    const char* const prog_name,
    const char* const search_path,
    const char* const search_pathext,
    const char* const root,
    char** const prog_path_out
) {
    if ( STR_IS_EMPTY(prog_name) ) { return 1; }

    return _locate_progam (
        NULL, NULL,
        IF_NULL_FALLBACK(search_path, getenv("PATH")),
        IF_NULL_FALLBACK(search_pathext, DEFAULT_SEARCH_PATHEXT),
        root, prog_name, prog_path_out
    );
}

char* which ( const char* const prog_name ) {
    char* prog;

    prog = NULL;
    if ( _any_which ( prog_name, &prog ) != 0 ) {
        x_free ( prog );
        return NULL;
    }
    return prog;
}

int qwhich ( const char* const prog_name ) {
    return _any_which ( prog_name, NULL );
}

char* whichmore (
    const char* const prog_name,
    const char* const search_path,
    const char* const search_pathext,
    const char* const root
) {
    char* prog;

    prog = NULL;
    if (
        _any_whichmore (
            prog_name, search_path, search_pathext, root, &prog
        ) != 0
    ) {
        x_free ( prog );
        return NULL;
    }
    return prog;
}

int qwhichmore (
    const char* const prog_name,
    const char* const search_path,
    const char* const search_pathext,
    const char* const root
) {
    return _any_whichmore (
        prog_name, search_path, search_pathext, root, NULL
    );
}



static int _locate_program__init_pathext_arr (
    const char* const restrict pathext_sep,
    const char* const restrict pathext,
    struct dynarray* const restrict pathext_arr_out,
    unsigned* const restrict have_pathext_out
) {
    *have_pathext_out = 0;

    if ( STR_IS_EMPTY(pathext) ) { return 0; }

    if ( dynarray_init ( pathext_arr_out, 10 ) != 0 ) { return -1; }

    if (
        str_split (
            pathext_arr_out,
            pathext,
            NULL,
            IF_NULL_FALLBACK(pathext_sep, DEFAULT_SEARCH_PATHEXT_SEP),
            -1
        ) < 0
    ) {
        dynarray_free ( pathext_arr_out );
        return -1;
    }

    if ( dynarray_argc(pathext_arr_out) < 1 ) {
        dynarray_free ( pathext_arr_out );
        /* *have_pathext_out = 0; */
    } else {
        *have_pathext_out = 1;
    }

    return 0;
}

static int _locate_program__init_search_path_arr (
    const char* const restrict search_path_sep,
    const char* const restrict search_path,
    struct dynarray* const search_path_arr_out
) {
    if ( dynarray_init ( search_path_arr_out, 10 ) !=  0 ) { return -1; }

    if ( STR_IS_EMPTY(search_path) ) {
        /* empty search path is OK */

    } else if (
        str_split (
            search_path_arr_out,
            search_path,
            NULL,
            IF_NULL_FALLBACK(search_path_sep, DEFAULT_SEARCH_PATH_SEP),
            -1
        ) < 0
    ) {
        dynarray_free ( search_path_arr_out );
        return -1;
    }

    return 0;
}

static int _locate_program__init_fspath_dynstr (
    struct dynstr_data* const restrict fspath_dstr,
    const char* const restrict root
) {
    if ( dynstr_data_init ( fspath_dstr, 64 ) != 0 ) { return -1; }

    /* apply root prefix, if non-empty and not "/" */
    if (
        STR_IS_NOT_EMPTY(root) && ( (*(root+1) != '\0') || (*root != '/') )
    ) {
        if ( dynstr_data_append_str_simple ( fspath_dstr, root ) != 0 ) {
            dynstr_data_free ( fspath_dstr );
            return -1;
        }

        /* assert dstr.current_size > 0 */
        if ( (fspath_dstr->data) [(fspath_dstr->current_size) - 1] != '/' ) {
            if ( dynstr_data_append_char ( fspath_dstr, '/' ) != 0 ) {
                dynstr_data_free ( fspath_dstr );
                return -1;
            }
        }
    }

    return 0;
}

static int _locate_program__try_prog_name_variants (
    struct dynstr_data* const restrict fspath_dstr,
    const size_t pathext_c,
    const char* const* const restrict pathext_v,
    const char* const restrict prog_name
) {
    /* append prog_name */
    if (
        (dynstr_data_append_str_simple ( fspath_dstr, prog_name ) != 0)
        || (dynstr_data_append_null ( fspath_dstr ) != 0)
    ) {
        return -1;
    }

    /* fspath is exefile? (i.e. no suffix) */
    if ( check_fs_exefile ( fspath_dstr->data ) == 0 ) {
        return WHICH_STATUS_FOUND_PROG;
    }

    /* pathext */
    {
        const size_t fspath_name_len_sav = fspath_dstr->current_size;
        size_t pathext_idx;

        for ( pathext_idx = 0; pathext_idx < pathext_c; pathext_idx++ ) {
            const char* const pathext = pathext_v [pathext_idx];

            if ( STR_IS_NOT_EMPTY(pathext) ) {
                if (
                    (dynstr_data_append_str_simple ( fspath_dstr, pathext ) != 0)
                    || (dynstr_data_append_null ( fspath_dstr ) != 0)
                ) {
                    return -1;
                }

                if ( check_fs_exefile ( fspath_dstr->data ) == 0 ) {
                    return WHICH_STATUS_FOUND_PROG;
                }
            }

            fspath_dstr->current_size = fspath_name_len_sav;
        }
    }

    return WHICH_STATUS_DID_NOT_FIND_PROG;
}

static int _locate_progam__abspath (
    struct dynstr_data* const restrict fspath_dstr,
    const size_t pathext_c,
    const char* const* const restrict pathext_v,
    const char* const restrict prog_name
) {
    const size_t fspath_dstr_prefix_len = fspath_dstr->current_size;

    const char* eff_prog_name;

    eff_prog_name = prog_name;
    if ( (fspath_dstr_prefix_len > 0) && (eff_prog_name != NULL) ) {
        while ( *eff_prog_name == '/' ) { eff_prog_name++; }
    }

    if ( STR_IS_EMPTY(eff_prog_name) ) {
        return WHICH_STATUS_BAD_USAGE;

    } else {
        return _locate_program__try_prog_name_variants (
            fspath_dstr, pathext_c, pathext_v, eff_prog_name
        );
    }
}

static int _locate_progam__name_in_search_path (
    struct dynstr_data* const restrict fspath_dstr,
    const size_t path_c,
    const char* const* const restrict path_v,
    const size_t pathext_c,
    const char* const* const restrict pathext_v,
    const char* const restrict prog_name
) {
    /* remember length of the root prefix (in the dynstr),
     * used for truncating the dynstr to that size on each loop
     * iteration without having to recreate to whole string.
     */
    const size_t fspath_dstr_prefix_len = fspath_dstr->current_size;

    size_t path_idx;
    const char* path;

    for ( path_idx = 0; path_idx < path_c; path_idx++ ) {
        path = path_v [path_idx];

        if ( path != NULL ) {
            /*
             * if there is a root prefix,
             * then dynstr already ends with "/",
             * so skip leading fspath separators.
             */
            if ( fspath_dstr_prefix_len > 0 ) {
                while ( *path == '/' ) { path++; }
            }

            if ( *path == '\0' ) {
                /* nop */

            } else if (
                (dynstr_data_append_str_simple ( fspath_dstr, path ) != 0)
                || (dynstr_data_append_char ( fspath_dstr, '/' ) != 0)
            ) {
                return -1;

            } else {
                switch (
                    _locate_program__try_prog_name_variants (
                        fspath_dstr, pathext_c, pathext_v, prog_name
                    )
                ) {
                    case WHICH_STATUS_FOUND_PROG:
                        return 0;

                    case WHICH_STATUS_DID_NOT_FIND_PROG:
                        break;

                    default:
                        /* try_prog_name_variants() does not return bad usage */
                        return -1;
                }
            }
        }

        fspath_dstr->current_size = fspath_dstr_prefix_len;
    }

    return WHICH_STATUS_DID_NOT_FIND_PROG;
}

static int _locate_progam (
    const char* const restrict search_path_sep,
    const char* const restrict pathext_sep,
    const char* const restrict search_path,
    const char* const restrict pathext,
    const char* const restrict root,
    const char* const restrict prog_name,
    char** const restrict prog_path_out
) {
    struct dynarray pathext_arr;
    struct dynstr_data dstr;
    unsigned have_pathext;
    int ret;

    if ( prog_path_out != NULL ) { *prog_path_out = NULL; }

    /* empty prog name makes no sense. */
    if ( STR_IS_EMPTY(prog_name) ) { return WHICH_STATUS_BAD_USAGE; }

    /* initialize str for [temporarily] storing prog path candidates */
    if ( _locate_program__init_fspath_dynstr ( &dstr, root ) != 0 ) {
        return -1;
    }

    /* initialize pathext array */
    if (
        _locate_program__init_pathext_arr (
            pathext_sep, pathext, &pathext_arr, &have_pathext
        ) != 0
    ) {
        dynstr_data_free ( &dstr );
        return -1;
    }


    /* must not return directly after this line */
    ret = -1;

    if ( *prog_name == '/' ) {
        /* prog name is a filesystem path */
        ret = _locate_progam__abspath (
            &dstr,
            (have_pathext ? dynarray_argc(&pathext_arr) : 0),
            (have_pathext ? dynarray_const_argv(&pathext_arr) : NULL),
            prog_name
        );

    } else {
        /* prog name is really a name */
        struct dynarray search_path_arr;

        /* which means that we need a non-empty search path! */
        if ( search_path == NULL ) {
            ret = WHICH_STATUS_BAD_USAGE;

        } else if ( *search_path == '\0' ) {
            ret = WHICH_STATUS_DID_NOT_FIND_PROG;

        /* and a search path array... */
        } else if (
            _locate_program__init_search_path_arr (
                search_path_sep, search_path, &search_path_arr
            ) != 0
        ) {
            ret = -1;

        } else {
            ret = _locate_progam__name_in_search_path (
                &dstr,
                dynarray_argc(&search_path_arr),
                dynarray_const_argv(&search_path_arr),
                (have_pathext ? dynarray_argc(&pathext_arr) : 0),
                (have_pathext ? dynarray_const_argv(&pathext_arr) : NULL),
                prog_name
            );

            /* free search path array */
            dynarray_free ( &search_path_arr );
        }
    }

    /* free pathext array */
    if ( have_pathext ) { dynarray_free ( &pathext_arr ); }

    /* transfer fspath dynstr or free it */
    if ( (ret == WHICH_STATUS_FOUND_PROG) && (prog_path_out != NULL) ) {
        dynstr_data_unref ( &dstr, prog_path_out );
    } else {
        dynstr_data_free ( &dstr );
    }

    return ret;
}
