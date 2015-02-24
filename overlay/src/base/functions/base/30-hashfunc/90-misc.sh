## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

if [ -n "${HASH_TYPE}" ]; then
hash_str() { hash="$( printf "%s" "${1}" | io_hash )"; [ -n "${hash}" ]; }

else
hash_str() { hash=; return @@EX_NOT_SUPPORTED@@; }
fi

io_hash_fspath()  { io_strip_fspath | io_hash; }
hash_fspath()     { strip_fspath | io_hash; }
