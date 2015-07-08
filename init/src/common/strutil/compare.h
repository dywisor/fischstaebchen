/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_STRUTIL_COMPARE_H_
#define _COMMON_STRUTIL_COMPARE_H_

#include <stdlib.h>

#include "../mac.h"

/**
 * Macro for checking whether a str is NULL or empty.
 *
 * @param s   str to check (may be NULL)
 *
 * @return 1 if str is empty else 0
 */
#define STR_IS_EMPTY(s)       ( (s == NULL) || (*s == '\0') )

/**
 * Macro for checking whether a str is neither NULL nor empty.
 *
 * @param s  str to check (may be NULL)
 *
 * @return 0 if str is empty else 1
 */
#define STR_IS_NOT_EMPTY(s)   ( (s != NULL) && (*s != '\0') )

/**
 * Macro that "aborts" a function (returns 1) if the given str is empty.
 *
 * @param s  str to check (may be NULL)
 *
 * @return <see description>
 */
#define RETFAIL_IF_STR_IS_EMPTY(s)     RETFAIL_IF_B_TRUE ( STR_IS_EMPTY(s) )

/** Macro that "aborts" a function (returns 1) if the given str is not empty.
 *
 * @param s  str to check (may be NULL)
 *
 * @return <see description>
 */
#define RETFAIL_IF_STR_IS_NOT_EMPTY(s) RETFAIL_IF_B_TRUE ( STR_IS_NOT_EMPTY(s) )

/**
 * Checks whether the given char means true or false.
 *
 * @param chr  a char
 *
 * @return 0 if chr means true, 1 if false, -1 if undecided
 */
__attribute__((const,warn_unused_result))
int char_means_true ( const char chr );

/**
 * Checks whether the given string means true or false.
 *
 * @param str   input str (should not be NULL)
 *
 * @return 0 if the input str means true, 1 if false, -1 if undecided
 */
__attribute__((warn_unused_result))
int str_means_true ( const char* const str );


/**
 * Checks a str for equality (case-sensitive).
 *
 * @param str         str to compare
 * @param <va_list>   NULL-terminated list of candidates
 *
 * @return 0 if any candidate matched str, else non-zero
 */
__attribute__((sentinel,warn_unused_result))
int _streq_any ( const char* const str, ... );
#define streq_any(str,...)  _streq_any(str,__VA_ARGS__,NULL)


/**
 * Checks a str for equality (case-insensitive).
 *
 * @param str         str to compare
 * @param <va_list>   NULL-terminated list of candidates
 *
 * @return 0 if any candidate matched str, else non-zero
 */
__attribute__((sentinel,warn_unused_result))
int _strcaseeq_any ( const char* const str, ... );
#define strcaseeq_any(str,...)  _strcaseeq_any(str,__VA_ARGS__,NULL)


/**
 * Checks a str for equality (case-sensitive) and returns the index
 * of the matching candidate. Useful for switch(...) statements.
 *
 *
 * @param str         str to compare
 * @param <va_list>   NULL-terminated list of candidates
 *
 * @return index of the matching candidate (>=0) or -1
 */
__attribute__((sentinel,warn_unused_result))
int _streq_switch ( const char* const str, ... );
#define streq_switch(str,...)  _streq_switch(str,__VA_ARGS__,NULL)

/** Shorthand for switch(streq_switch(...)) */
#define STREQ_SWITCH(s,...) switch(streq_switch(s,__VA_ARGS__))

/**
 * Checks a str for equality (case-insensitive) and returns the index
 * of the matching candidate. Useful for switch(...) statements.
 *
 *
 * @param str         str to compare
 * @param <va_list>   NULL-terminated list of candidates
 *
 * @return index of the matching candidate (>=0) or -1
 */
__attribute__((sentinel,warn_unused_result))
int _strcaseeq_switch ( const char* const str, ... );
#define strcaseeq_switch(str,...)  _strcaseeq_switch(str,__VA_ARGS__,NULL)

/** Shorthand for switch(strcaseeq_switch(...)) */
#define STRCASEEQ_SWITCH(s,...) switch(strcaseeq_switch(s,__VA_ARGS__))

/**
 * Checks whether a str starts with a certain char sequence (case-sensitive).
 *
 * @param str         str to check (must not be NULL)
 * @param prefix      prefix       (must not be NULL)
 *
 * @return pointer to the remainder (may be empty, "")
 *         if str starts with prefix, else NULL
 */
__attribute__((warn_unused_result))
const char* str_startswith     ( const char* const str, const char* const prefix );

/**
 * Checks whether a str starts with a certain char sequence (case-insensitive).
 *
 * @param str         str to check (must not be NULL)
 * @param prefix      prefix       (must not be NULL)
 *
 * @return pointer to the remainder (may be empty, "")
 *         if str starts with prefix, else NULL
 */
__attribute__((warn_unused_result))
const char* str_casestartswith ( const char* const str, const char* const prefix );

#endif /* _COMMON_STRUTIL_COMPARE_H_ */
