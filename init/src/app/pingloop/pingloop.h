/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _APP_PINGLOOP_H_
#define _APP_PINGLOOP_H_

#include <stdint.h>

int pingloop ( const char* const remote, const uint8_t interval );

#endif /* _APP_PINGLOOP_H_ */
