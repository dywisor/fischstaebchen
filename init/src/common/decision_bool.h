/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_DECISION_BOOL_H_
#define _COMMON_DECISION_BOOL_H_

enum {
   DBOOL_UNDECIDED = 0x0,
   DBOOL__FORCED   = 0x1,
   DBOOL_FALSE     = 0x2,
   DBOOL_TRUE      = 0x4,

   DBOOL_FORCED_FALSE = (DBOOL_FALSE | DBOOL__FORCED),
   DBOOL_FORCED_TRUE  = (DBOOL_TRUE  | DBOOL__FORCED),

   DBOOL__MASK = (DBOOL__FORCED | DBOOL_FALSE | DBOOL_TRUE)
};

#define DBOOL_TYPE  unsigned


#define dbool_is_undecided(v)     (((v) & (DBOOL_FALSE | DBOOL_TRUE)) == 0)
#define dbool_is_false(v)         (((v) & DBOOL_FALSE) != 0)
#define dbool_is_true(v)          (((v) & DBOOL_TRUE)  != 0)

#define dbool_is_undef_true(v)    (((v) & DBOOL_FALSE) == 0)
#define dbool_is_undef_false(v)   (((v) & DBOOL_TRUE)  == 0)

#define dbool_is_forced_false(v)  \
   (((v) & DBOOL_FORCED_FALSE) == DBOOL_FORCED_FALSE)
#define dbool_is_forced_true(v)   \
   (((v) & DBOOL_FORCED_TRUE) == DBOOL_FORCED_TRUE)

#define dbool_is_clean(v)  (((v) & ~DBOOL__MASK) == 0)
#define dbool_is_dirty(v)  (((v) & ~DBOOL__MASK) != 0)

#define dbool_enforce(v)           do { (v) |= DBOOL__FORCED;      } while(0)
#define dbool_set_undecided(v)     do { (v)  = DBOOL_UNDECIDED;    } while(0)
#define dbool_set_false(v)         do { (v)  = DBOOL_FALSE;        } while(0)
#define dbool_set_true(v)          do { (v)  = DBOOL_TRUE;         } while(0)
#define dbool_set_forced_false(v)  do { (v)  = DBOOL_FORCED_FALSE; } while(0)
#define dbool_set_forced_true(v)   do { (v)  = DBOOL_FORCED_TRUE;  } while(0)


/*
__attribute__((const, warn_unused_result))
static inline DBOOL_TYPE dbool_from_xint ( const int i ) {
   return (
      ((i) == 0)
         ? DBOOL_TRUE
         : ( ((i) > 0) ? DBOOL_FALSE : DBOOL_UNDECIDED )
   );
}

#define dbool_from_bool(b)  ((b) ? DBOOL_TRUE : DBOOL_FALSE)
*/

#endif  /* _COMMON_DECISION_BOOL_H_ */
