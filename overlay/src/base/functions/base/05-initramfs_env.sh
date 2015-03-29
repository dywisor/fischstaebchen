## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

. "@@ENVFILE@@" || exit

if [ -f "${IENV:-/}" ]; then
   . "${IENV}" || exit
fi

if test -f "${ICOLORS:-/}"; then
   ## ( . file; ) && . file || NO_COLOR=y
   . "${ICOLORS}" || exit
   HAVE_ICOLORS=y
else
   : ${NO_COLOR:=y}
fi

. "@@SHFUNC_LIBDIR@@/base.sh" || exit
