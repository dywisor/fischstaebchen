/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_FS_CONSTANTS_H_
#define _COMMON_FS_CONSTANTS_H_

#include <sys/types.h>
#include <sys/stat.h>

enum {
   /** all permission bits */
   PERMISSION_MASK = (
      S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO
   ),

   /* 0111, 0222, 0444 */
   X_ALL        = ( S_IXUSR | S_IXGRP | S_IXOTH ),
   W_ALL        = ( S_IWUSR | S_IWGRP | S_IWOTH ),
   R_ALL        = ( S_IRUSR | S_IRGRP | S_IROTH ),

   /* 0600, 0060, 0006 */
   RW_USR       = ( S_IRUSR | S_IWUSR ),
   RW_GRP       = ( S_IRGRP | S_IWGRP ),
   RW_OTH       = ( S_IROTH | S_IWOTH ),

   /* 0500, 0050, 0005 */
   RX_USR       = ( S_IRUSR | S_IXUSR ),
   RX_GRP       = ( S_IRGRP | S_IXGRP ),
   RX_OTH       = ( S_IROTH | S_IXOTH ),

   /* 0666 */
   RW_RW_RW     = ( RW_USR | RW_GRP | RW_OTH ),

   /* 0660 */
   RW_RW        = ( RW_USR | RW_GRP ),

   /* 0664 */
   RW_RW_RO     = ( RW_RW | S_IROTH ),

   /* 0644 */
   RW_RO_RO     = ( RW_USR | S_IRGRP | S_IROTH ),

   /* 0640 */
   RW_RO        = ( RW_USR | S_IRGRP ),

   /* 0620 */
   RW_WO        = ( RW_USR | S_IWGRP ),

   /* 0711 */
   RWX_X_X      = ( RW_USR | X_ALL ),

   /* 0755 */
   RWX_RX_RX    = ( RW_RO_RO | X_ALL ),

   /* 0777 */
   RWX_RWX_RWX  = ( RW_RW_RW | X_ALL ),

   /* 1777 */
   SRWX_RWX_RWX = ( RWX_RWX_RWX | S_ISVTX ),

   /* default directory permissions */
   DEFDIRPERM   = RWX_RX_RX,
   /* default binary permissions */
   DEFBINPERM   = RWX_RX_RX,
   /* default permissions for mountpoint directories */
   DEFMPDIRPERM = RX_USR
};

#endif /* _COMMON_FS_CONSTANTS_H_ */
