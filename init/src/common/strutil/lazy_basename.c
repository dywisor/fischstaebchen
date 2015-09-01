#include <stdlib.h>

#include "lazy_basename.h"

const char* lazy_basename ( const char* const input_str ) {
    static const char* const EMPTY_STR;

    const char* iter;
    const char* p_basename;

    if ( (input_str == NULL) || (*input_str == '\0') ) { return EMPTY_STR; }

    p_basename = input_str;
    for ( iter = input_str; *iter != '\0'; iter++ ) {
        switch ( *iter ) {
            case '/':
                /* set basename to iter+1, which may point an empty str */
                p_basename = (iter + 1);
                break;
        }
    }

    return (*p_basename != '/') ? p_basename : EMPTY_STR;
}
