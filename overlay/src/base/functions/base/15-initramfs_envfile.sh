## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

__add_env()   { __enfile_add     "${IENV:?}" "${@}"; }
add_env()     { envfile_add      "${IENV:?}" "${@}"; }
add_env_var() { envfile_add_var  "${IENV:?}" "${@}"; }
