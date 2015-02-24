/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_BASELAYOUT_MDEV_H_
#define _COMMON_BASELAYOUT_MDEV_H_

int setup_mdev (void);
int unregister_mdev (void);
int mdev_run_scan (void);

#endif /* _COMMON_BASELAYOUT_MDEV_H_ */
