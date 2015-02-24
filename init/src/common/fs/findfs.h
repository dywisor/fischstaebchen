/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_FINDFS_H_
#define _COMMON_FS_FINDFS_H_

/**
 * Runs the (external) "findfs" program and stores the resolved device
 * in dev_out.
 *
 * @param disk_spec    disk str, e.g. "LABEL=rootfs"
 * @param dev_out
 *
 * @return 0 on success, else non-zero
 */
int findfs ( const char* const disk_spec, char** const dev_out );

#endif /* _COMMON_FS_FINDFS_H_ */
