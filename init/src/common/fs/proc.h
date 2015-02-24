/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_PROC_H_
#define _COMMON_FS_PROC_H_

/** data struct for entries read from /proc/misc */
struct proc_misc_chardev_info {
   /** device name */
   const char* name;
   /** minor device number */
   unsigned    min;
};

/**
 * callback function for proc_misc_chardev_minor_foreach()
 *
 * @param <struct proc_misc_chardev_info*>  entry from /proc/misc
 * @param <void*>    additional data (supplied by the caller)
 *
 * @return 0 if proc_misc_chardev_minor_foreach() should continue
 *         with the next entry, else non-zero
 */
typedef int (*proc_misc_chardev_callback) (
   const struct proc_misc_chardev_info* p_info, void* data
);

/**
 * Reads the minor device number of a "misc" device from /proc/misc.
 *
 * @param p_info  half-initialized struct proc_get_misc_chardev_minor
 *                name should be set and not empty
 *
 * @return  0 if entry found, 1 if not, -1 on error
 */
int proc_get_misc_chardev_minor_struct (
   struct proc_misc_chardev_info* const p_info
);

/**
 * Reads the minor device number of a "misc" device (specified by name)
 * from /proc/misc and stores it in p_min.
 *
 * @param name    name of the misc device
 * @param p_min   int pointer for storing the minor device number
 *
 * @return  0 if entry found, 1 if not, -1 on error
 */
int proc_get_misc_chardev_minor (
   const char* const name, unsigned* const p_min
);

/**
 * Reads /proc/misc and calls the callback function for each entry.
 *
 * @param callback  function that processes struct proc_misc_chardev_info items
 * @param data      additional data (may be NULL)
 *
 * @return -1 on error, else first non-zero retcode from callback (...or 0)
 */
int proc_misc_chardev_minor_foreach (
   proc_misc_chardev_callback callback, void* const data
);

#endif /* _COMMON_FS_PROC_H_ */
