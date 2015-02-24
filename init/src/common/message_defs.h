/*
 * Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
 *
 * Distributed under the terms of the MIT license.
 * (See LICENSE.MIT or http://opensource.org/licenses/MIT)
 */

#ifndef _COMMON_MESSAGE_DEFS_H_
#define _COMMON_MESSAGE_DEFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define _MSG_CREATE_COLOR(c) "\033[" c "m"
#define MSG_COLOR_DEFAULT       _MSG_CREATE_COLOR("0")
#define MSG_COLOR_BLACK         _MSG_CREATE_COLOR("1;30")
#define MSG_COLOR_RED           _MSG_CREATE_COLOR("1;31")
#define MSG_COLOR_GREEN         _MSG_CREATE_COLOR("1;32")
#define MSG_COLOR_YELLOW        _MSG_CREATE_COLOR("1;33")
#define MSG_COLOR_BLUE          _MSG_CREATE_COLOR("1;34")
#define MSG_COLOR_MAGENTA       _MSG_CREATE_COLOR("1;35")
#define MSG_COLOR_CYAN          _MSG_CREATE_COLOR("1;36")
#define MSG_COLOR_WHITE         _MSG_CREATE_COLOR("1;37")

#define MSG_COLOR_BLACK_NB      _MSG_CREATE_COLOR("0;30")
#define MSG_COLOR_RED_NB        _MSG_CREATE_COLOR("0;31")
#define MSG_COLOR_GREEN_NB      _MSG_CREATE_COLOR("0;32")
#define MSG_COLOR_YELLOW_NB     _MSG_CREATE_COLOR("0;33")
#define MSG_COLOR_BLUE_NB       _MSG_CREATE_COLOR("0;34")
#define MSG_COLOR_MAGENTA_NB    _MSG_CREATE_COLOR("0;35")
#define MSG_COLOR_CYAN_NB       _MSG_CREATE_COLOR("0;36")
#define MSG_COLOR_WHITE_NB      _MSG_CREATE_COLOR("0;37")

/* these colors make users scream! */
#define MSG_COLOR_BLACK_BG      _MSG_CREATE_COLOR("1;37;100")
#define MSG_COLOR_RED_BG        _MSG_CREATE_COLOR("1;37;101")
#define MSG_COLOR_GREEN_BG      _MSG_CREATE_COLOR("1;37;102")
/* favorite: */
#define MSG_COLOR_YELLOW_BG     _MSG_CREATE_COLOR("1;37;103")
#define MSG_COLOR_BLUE_BG       _MSG_CREATE_COLOR("1;37;104")
#define MSG_COLOR_MAGENTA_BG    _MSG_CREATE_COLOR("1;37;105")
#define MSG_COLOR_CYAN_BG       _MSG_CREATE_COLOR("1;37;106")
#define MSG_COLOR_WHITE_BG      _MSG_CREATE_COLOR("1;30;107")


#define MSG_GET_COLOR(cref)  MSG_COLOR_ ## cref


#endif /* _COMMON_MESSAGE_DEFS_H_ */
