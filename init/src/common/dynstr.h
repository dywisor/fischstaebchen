/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_DYNSTR_H_
#define _COMMON_DYNSTR_H_

#include <stdlib.h>
#include <sys/types.h>

#ifndef DEFAULT_DYNSTR_SIZE
enum {
   /** initial capacity of a dynstr_data object */
   DEFAULT_DYNSTR_SIZE = 32
};
#endif

/**
 * dynamically allocated data object type
 */
struct dynstr_data {
   /** pointer to the actual str */
   char* data;
   /** number of currently used objects */
   size_t current_size;
   /** current capacity of the the data object */
   size_t max_size;
};


/**
 * Initializes a dynamically allocated str data object.
 *
 * @param pdata          pointer to a dynstr struct (should not be NULL)
 * @param initial_size   initial capacity of the data structure (>= 0)
 *
 * @return 0 on success, else non-zero
 */
__attribute__((warn_unused_result))
int dynstr_data_init (
   struct dynstr_data* const pdata,
   const size_t initial_size
);

/**
 * Resizes a dynamically allocated str data object so that it has enough room
 * for at least min_size chars. No-op if there's already enough space
 * available.
 *
 * Frees the data if reallocation failed.
 *
 * @param pdata     pointer to a dynstr struct (must not be NULL)
 * @param min_size  requested size
 *
 * @return 0 on success, else non-zero
 */
__attribute__((warn_unused_result))
int dynstr_data_resize (
   struct dynstr_data* const pdata, const size_t min_size
);

/**
 * Resizes a dynamically allocated str data object so that it has enough room
 * for at least current_size + num_size chars.
 * No-op if there's already enough space available.
 *
 * Frees the data if reallocation failed.
 *
 * @param pdata       pointer to a dynstr struct (must not be NULL)
 * @param num_items   number of additional objects that should fit into
 *                    the dynstr struct (>0)
 *
 * @return 0 on success, else non-zero
 */
__attribute__((warn_unused_result))
int dynstr_data_grow (
   struct dynstr_data* const pdata, const size_t num_items
);

/**
 * Detaches the str from a dynamically allocated str data object (by setting
 * it to NULL) and resets the size counters.
 * Optionally stores the str in the given data_out pointer (if not NULL).
 *
 * @param pdata     pointer to a dynstr struct (must not be NULL)
 * @param data_out  pointer to a data pointer or NULL
 *                   *data_out should point to NULL
 */
void dynstr_data_unref ( struct dynstr_data* const pdata, char** const str_out );

/**
 * Destructs a dynamically allocated str object, freeing its memory.
 *
 * @param pdata  pointer to a dynstr struct (must not be NULL)
 */
void dynstr_data_free ( struct dynstr_data* const pdata );

/**
 * Appends a str to a dynamically allocated str data object.
 * Optionally converts chars using the given function.
 * Chars for which the function returns NULL get filtered out.
 *
 * NOTE: may allocate considerably more space than required if convert_char()
 *       filters out many..all chars.
 *
 * @param pdata         pointer to a dynstr struct (must not be NULL)
 * @param str_len       length of the input str
 * @param pstr          input str
 * @param convert_char  function pointer or NULL
 *
 * @return 0 on success, else non-zero
 */
int dynstr_data_append_str (
   struct dynstr_data* const pdata,
   const size_t str_len,
   const char* const pstr,
   const char* (*convert_char) (size_t, const char*)
);

/**
 * Same as dynstr_data_append_str(pdata, strlen(pstr), pstr, NULL).
 * (= just append that str.)
 */
int dynstr_data_append_str_simple (
   struct dynstr_data* const pdata,
   const char* const pstr
);

/**
 * Like dynstr_data_append_str_simple(), but appends a "join sequence"
 * (another string) before pstr if the dynstr is not empty.
 */
int dynstr_data_sjoin_append (
   struct dynstr_data* const pdata,
   const char* const join_seq, const size_t join_seq_len,
   const char* const pstr
);

/**
 * Like dynstr_data_append_str_simple(),
 * but appends a single char before pstr if the dynstr is not empty.
 */
int dynstr_data_cjoin_append (
   struct dynstr_data* const pdata,
   const char join_chr,
   const char* const pstr
);

/**
 * Appends a single char to a dynamically allocated str data object.
 *
 * @param pdata  pointer to a dynstr struct (must not be NULL)
 * @param chr    char to append
 *
 * @return 0 on success, else non-zero
 */
int dynstr_data_append_char ( struct dynstr_data* const pdata, const char chr );

/**
 * Removes whitespace characters and filters non-printable chars at the end
 * of the given dynstr object.
 *
 * @param pdata   pointer to a dynstr struct (must not be NULL)
 *
 * @return 0 on success, else non-zero
 */
int dynstr_data_rstrip ( struct dynstr_data* const pdata );

/**
 * Returns the str stored in a struct dynstr_data*.
 *
 * Likely to be removed or replaced by a macro in future.
 */
char* dynstr_data_get ( struct dynstr_data* pdata );

/**
 * Sets the first char of a dynstr to '\0' and resets its length to 0.
 * This effectively "empties" the str (--insecure--).
 *
 * @param pdata
 *
 * @return always 0
 */
int dynstr_data_truncate ( struct dynstr_data* pdata );

/**
 * Appends a null char '\0' to a dynstr.
 * Resizes the str if necessary.
 *
 * @param pdata
 *
 * @return 0 on success, else non-zero
 */
int dynstr_data_append_null ( struct dynstr_data* pdata );

#endif /* _COMMON_DYNSTR_H_ */
