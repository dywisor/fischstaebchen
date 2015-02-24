/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_DYNARRAY_H_
#define _COMMON_DYNARRAY_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>

enum {
   DYNARRAY_IS_CONST = 0x1
};

#ifndef DYNARRAY_DEFAULT_INITIAL_CAPACITY
enum { DYNARRAY_DEFAULT_INITIAL_CAPACITY = 1 };
#endif

/**
 * dynamically allocated array - data type
 */
struct dynarray {
   /** the actual array */
   void**       arr;
   /** length of the array (used size) */
   size_t       len;
   /** capacity of the array (allocated size) */
   size_t       size;
   /** flags, e.g. "has const data" */
   unsigned     flags;
};

/**
 * Initializes a dynamically allocated array.
 *
 * @param p_darr          pointer to the dynarray (must not be NULL)
 * @param initial_size    initial capacity (0 for default)
 *
 * @return 0 on success, else non-zero
 */
int dynarray_init ( struct dynarray* const p_darr, const size_t initial_size );

/**
 * Creates a new dynarray of the requested size.
 *
 * @param initial_size
 *
 * @return pointer to dynarray or NULL
 */
struct dynarray* new_dynarray ( const size_t initial_size );

/**
 * Sets or unsets a dynarray's "readonly data" flag.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param status  intbool (0 => unset flag, else: set "readonly" flag)
 */
void dynarray_set_data_readonly (
   struct dynarray* const p_darr, const int status
);

/**
 * Resizes a dynarray so that it has enough space for at least want_len items.
 * No-op if the array's capacity is already sufficient.
 *
 * @param p_darr    pointer to the dynarray (must not be NULL)
 * @param want_len
 *
 * @return 0 on success, else non-zero
 */
int dynarray_resize ( struct dynarray* const p_darr, const size_t want_len );

/**
 * Resizes a dynarray so that it has enough space for at least one more item.
 * No-op if the array is already big enough.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 *
 * @return 0 on success, else non-zero
 */
int dynarray_grow ( struct dynarray* const p_darr );

/**
 * Empties a dynarray.
 *
 * Releases the items of a dynarray by setting them to NULL and resets the
 * length (but not the capacity) to 0.
 * The items get freed if the "readonly data" flag is not set.
 *
 * @param p_darr  pointer to the dynarray (may be NULL)
 *
 * @return 0
 */
int dynarray_release ( struct dynarray* const p_darr );

/**
 * Destroys a dynarray.
 *
 * Empties the dynarray and frees the actual array.
 *
 * @param p_darr  pointer to the dynarray (may be NULL)
 *
 * @return 0
 */
int dynarray_free ( struct dynarray* const p_darr );

void dynarray_free_ptr ( struct dynarray** const p_darr );

/**
 * Appends arbitrary data to a dynarray.
 *
 * Reallocates the array if necessary.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param data    data to append
 *
 * @return 0 on success, else non-zero
 */
int dynarray_append ( struct dynarray* const p_darr, void* const data );

/**
 * Appends a copy of the given str to a dynarray.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param str     str
 *
 * @return 0 on success, else non-zero
 */
int dynarray_append_strdup (
   struct dynarray* const p_darr, const char* const str
);

/**
 * Appends a refcopy of the given str to a dynarray.
 *
 * This is identical to calling dynarray_append(p_darr,str),
 * but handles typecasting.
 *
 * This operation is UNSAFE.
 * The dynarray's should be marked as readonly.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param str     str
 *
 * @return 0 on success, else non-zero
 */
int dynarray_append_strnodup (
   struct dynarray* const p_darr, const char* const str
);

/**
 * Copies items from a str array to the dynarray.
 *
 * Skips NULL items.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param argc    number of items (length of argv)
 * @param argv    str array
 *
 * @return 0 on success, else non-zero
 */
int dynarray_extend_strdup (
   struct dynarray* const p_darr,
   const int argc, const char* const* const argv
);

/**
 * Adds refcopies to items from the given str array to a dynarray.
 *
 * Skips NULL items.
 *
 * @param p_darr  pointer to the dynarray (must not be NULL)
 * @param argc    number of items (length of argv)
 * @param argv    str array
 *
 * @return 0 on success, else non-zero
 */
int dynarray_extend_strnodup (
   struct dynarray* const p_darr,
   const int argc, const char* const* const argv
);

/**
 * Removes the last entry of a dynarry and stores it in data_out (if not NULL).
 *
 * Frees the removed item if data_out is NULL and the "readonly data" flag
 * is not set. *
 *
 * @param p_darr    pointer to the dynarray (should not be NULL)
 * @param data_out  NULL or a pointer "container" (<type>**)
 *
 * @return 0 on success, else non-zero
 */
int dynarray_pop ( struct dynarray* const p_darr, void** const data_out );


/**
 * Transfers items from a NULL-terminated [const] char* va_list to a dynarray.
 *
 * Does not set the "readonly data" flag (needs to be done manually).
 * The terminating NULL pointer doesn't get appended to the array.
 *
 * @param p_darr
 * @param vargs
 * @param copy_items    intbool that controls whether items should be added
 *                       as reference (0) or as copy (1 (!=0)).
 *
 * @return 0 on success, else non-zero
 */
int va_list_to_dynarray (
   struct dynarray* const p_darr, va_list vargs, const int copy_items
);

/**
 * Transfers items from a NULL-terminated [const] char* var args list to
 * a dynarray.
 *
 * See va_list_to_dynarray() for details.
 *
 * @param p_darr
 * @param copy_items
 * @param <va_list>
 *
 * @return 0 on success, else non-zero
 */
int _varargs_to_dynarray (
   struct dynarray* const p_darr, const int copy_items, ...
);
#define varargs_to_dynarray(...) _varargs_to_dynarray(__VA_ARGS__,NULL)

#define dynarray_set(d,k,v)               ((d)->arr)[k] = v;
#define dynarray_get(d,k)                 (((d)->arr)[k])
#define dynarray_get_as(d,k,t)            (t)(dynarray_get(d,k))
#define dynarray_get_str(d,k)             dynarray_get_as(d,k,char*)

#define dynarray_argc(d)                  ((d)->len)
#define dynarray_argv(d)                  (char**)((d)->arr)
#define dynarray_const_argv(d)            (const char**)((d)->arr)

#define _dynarray_shift(d,o)              (((d)->arr)+o)

#define dynarray_argc_shift(d,o)          (((d)->len)-o)
#define dynarray_argv_shift(d,o)          (char**)(_dynarray_shift(d,o))
#define dynarray_const_argv_shift(d,o)    (const char**)(_dynarray_shift(d,o))

#define dynarray_foreach(d, idx_var)  \
   for ( idx_var = 0; idx_var < dynarray_argc(d); idx_var++ )

#define dynarray_foreach_reversed(d, idx_var)  \
   for ( idx_var = ( dynarray_argc(d) - 1 ); idx_var >= 0; idx_var-- )


#define dynarray_destruct_as(d, destructor_func, item_type)  \
   do { \
      size_t iter; \
      \
      dynarray_foreach ( d, iter ) { \
         destructor_func ( dynarray_get_as ( d, iter, item_type ) ); \
         dynarray_set ( d, iter, NULL ); \
      } \
   } while(0)

#endif /* _COMMON_DYNARRAY_H_ */
