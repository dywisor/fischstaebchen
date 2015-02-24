/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_STAT_H_
#define _COMMON_FS_STAT_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Checks whether the permission bits of a struct stat match the given mode.
 *
 * @param p_stat
 * @param mode
 *
 * @return 0 if matched, else non-zero
 */
int stat_permissions_eq (
   const struct stat* const p_stat, const mode_t mode
);

/**
 * Checks whether the uid of a struct stat matches the given uid.
 *
 * @param p_stat
 * @param uid
 *
 * @return 0 if matched, else non-zero
 */
int stat_uid_eq (
   const struct stat* const p_stat, const uid_t uid
);

/**
 * Checks whether the gid of a struct stat matches the given gid.
 *
 * @param p_stat
 * @param gid
 *
 * @return 0 if matched, else non-zero
 */
int stat_gid_eq (
   const struct stat* const p_stat, const gid_t gid
);

/**
 * Checks whether the uid/gid of a struct stat match the given uid/gid.
 *
 * @param p_stat
 * @param uid
 * @param gid
 *
 * @return 0 if matched, else non-zero
 */
int stat_owner_eq (
   const struct stat* const p_stat, const uid_t uid, const gid_t gid
);

/**
 * Chmods fspath if its mode does not match the requested mode.
 *
 * @param fspath
 * @param p_stat
 * @param mode
 *
 * @return 0 on success, else non-zero
 */
int chmod_stat (
   const char* const fspath,
   const struct stat* const p_stat,
   const mode_t mode
);

/**
 * Chowns fspath if its uid/gid do not match the requested uid/gid.
 *
 * @param fspath
 * @param p_stat
 * @param uid
 * @param gid
 *
 * @return 0 on success, else non-zero
 */
int chown_stat (
   const char* const fspath,
   const struct stat* const p_stat,
   const uid_t uid, const gid_t gid
);

/**
 * Chmods and/or chowns fspath if its mode/uid/gid do not match
 * the requested mode/uid/gid.
 *
 * @param fspath
 * @param p_stat
 * @param mode
 * @param uid
 * @param gid
 *
 * @return 0 on success, else non-zero
 */
int chmod_chown_stat (
   const char* const fspath,
   const struct stat* const p_stat,
   const mode_t mode, const uid_t uid, const gid_t gid
);

#endif /* _COMMON_FS_STAT_H_ */
