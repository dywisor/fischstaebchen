/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_PROCESS_INFO_H_
#define _COMMON_FS_PROCESS_INFO_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "../data_types/dynarray.h"


enum {
    ENVIRON_VARNAME_VALUE_PAIR_PARSE_SUCCESS = 0,
    ENVIRON_VARNAME_VALUE_PAIR_PARSE_NOVALUE = 1,

    ENVIRON_VARNAME_VALUE_PAIR_PARSE__LAST
};

struct environ_varname_value_pair {
   char*       _buf;
   char*       _split_pos;
   const char* varname;
   const char* value;
};


/**
 * Initializes an environ varname,value pair data struct.
 *
 * @param evp                pointer to environ_varname_value_pair struct
 *                           (should not be NULL, must be uninitialized)
 * @param varname_value_str  either NULL or a "<varname>=[<value>]" str
 *                            Passing a str here is identical to
 *                            init(...,NULL) followed by set(...,str)
 *
 * @return 0 when successful and non-zero otherwise
 */
int environ_varname_value_pair_init (
    struct environ_varname_value_pair* const evp,
    const char* const varname_value_str
);

/**
 * Frees the members of an environ varname,value pair data struct.
 *
 * @param evp                pointer to environ_varname_value_pair struct
 *                           (may be NULL Xor must be initialized)
 */
void environ_varname_value_pair_free (
    struct environ_varname_value_pair* const evp
);

/**
 * Frees an environ varname,value pair data struct (including its members).
 *
 * @param p_evp              pointer to environ_varname_value_pair struct pointer
 *                           (may be NULL Xor must be initialized)
 */
void environ_varname_value_pair_free_ptr (
    struct environ_varname_value_pair** const p_evp
);

/**
 * Parses a "<varname>=[<value>]" str and stores the information in a
 * environ varname,value pair data struct (which gets reinitialized).
 *
 * @param evp                pointer to environ_varname_value_pair struct
 *                           (should not be NULL, must be initialized)
 * @param varname_value_str  "<varname>=[<value>]" str
 *
 * @return 0 when successful, > 0 for parser errors, < 0 for severe errors
 */
int environ_varname_value_pair_set (
    struct environ_varname_value_pair* const evp,
    const char* const varname_value_str
);

/**
 * Sets dynarray item destructor to a function suitable for freeing
 * environ_varname_value_pair data structs.
 *
 *
 * @param p_darr            dynarray (should not be NULL)
 */
void environ_varname_value_pair_set_dynarray_destructor (
    struct dynarray* const restrict p_darr
);

/**
 * Converts an array of "<varname>=<value>" strings to
 * environ_varname_value_pair data structs and stores them in a dynarray.
 *
 * @param argc              number of strings to convert,
 *                          usually length of argv
 * @param argv              array of "<varname>=<value>" strings
 * @param evp_arr_out       dynarray for storing environ_varname_value_pair
 *                          data structs
 *
 * @return 0 when successful,
 *         > 0 for parser errors (keeps going),
 *         < 0 for severe errors
 */
int environ_varname_value_pair_convert_array (
    const size_t argc,
    const char* const* const restrict argv,
    struct dynarray* const restrict evp_arr_out
);

/**
 * Parsed a "<varname>=[<value>]" str and stores the varname and value
 * in two distinct variables.
 *
 * This is basically identical to the following code snippet
 * (+/- error checking, corner cases)::
 *
 *     struct environ_varname_value_pair evp;
 *     environ_varname_value_pair_init ( &evp, varname_value_str );
 *     *varname_out = strdup ( evp.varname );  // strdup(), because evp stores
 *     *value_out   = strdup ( evp.value );    // varname and value in a single str
 *     environ_varname_value_pair_free ( &evp )
 *
 *
 * @param varname_value_str
 * @param varname_out
 * @param value_out
 *
 * @return 0 when successful, > 0 for parser errors, < 0 for severe errors
 */
int split_environ_varname_value_str (
    const char* const restrict varname_value_str,
    char** const restrict      varname_out,
    char** const restrict      value_out
);


/**
 * Reads a null-separated cmdline file (/proc/<pid>/cmdline) and stores
 * its fields in a dynarray.
 *
 * @param cmdline_filepath  path to the cmdline file (should not be NULL)
 * @param p_darr            dynarray for storing argv read from cmdline file
 *                          (must not be NULL and must be initialized)
 *
 * @return 0 when successful and -1 otherwise
 */
int proc_read_cmdline_from_file (
   const char* const cmdline_filepath,
   struct dynarray* const p_darr
);

/**
 * Reads the null-separated cmdline file of a process specified by its
 * process id and stores the argv in a dynarray.
 *
 * @param pid               process id
 * @param p_darr            dynarray for storing argv read from cmdline file
 *                          (must not be NULL and must be initialized)
 *
 * @return 0 when successful and -1 otherwise
 */
int proc_read_cmdline_from_pid (
   const pid_t cmdline_pid,
   struct dynarray* const p_darr
);

/**
 * Reads a already opened, null-separated cmdline file
 * and stores the argv in a dynarray.
 *
 * @param fd                file descriptor of the opened cmdline file
 * @param p_darr            dynarray for storing argv read from cmdline file
 *                          (must not be NULL and must be initialized)
 *
 * @return 0 when successful and -1 otherwise
 */
int proc_read_cmdline_from_fd (
    const int fd,
    struct dynarray* const p_darr
);


/**
 * Reads a null-separated environ file (/proc/<pid>/environ) and stores
 * its data in a dynarray.
 *
 * @param environ_filepath  path to the cmdline file (should not be NULL)
 * @param p_darr            dynarray for storing strings read from the
 *                          environ file
 *                          (must not be NULL and must be initialized)
 *
 * @return 0 when successful and -1 otherwise
 */
int proc_read_environ_from_file (
   const char* const environ_filepath,
   struct dynarray* const p_darr
);

/**
 * Reads the null-separated environ file of a process specified by its
 * process id and stores the data in a dynarray.
 *
 * @param pid               process id
 * @param p_darr            dynarray
 *                          (must not be NULL and must be initialized)
 *
 * @return 0 when successful and -1 otherwise
 */
int proc_read_environ_from_pid (
   const pid_t pid,
   struct dynarray* const p_darr
);

/**
 * Reads a already opened, null-separated environ file
 * and stores the data in a dynarray.
 *
 * @param fd                file descriptor of the opened environ file
 * @param p_darr            dynarray
 *
 * @return 0 when successful and -1 otherwise
 */
int proc_read_environ_from_fd (
    const int fd,
    struct dynarray* const p_darr
);

#endif  /* _COMMON_FS_PROCESS_INFO_H_ */
