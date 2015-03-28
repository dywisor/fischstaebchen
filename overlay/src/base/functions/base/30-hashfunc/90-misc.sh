## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if [ -n "${HASH_TYPE}" ]; then
# shellcheck disable=SC2119
hash_str() { hash="$( printf "%s" "${1}" | io_hash )"; [ -n "${hash}" ]; }

else
# shellcheck disable=SC2119
hash_str() { hash=; return @@EX_NOT_SUPPORTED@@; }
fi

# shellcheck disable=SC2119
io_hash_fspath()  { io_strip_fspath | io_hash; }
# shellcheck disable=SC2119
hash_fspath()     { strip_fspath | io_hash; }
