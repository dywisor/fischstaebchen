## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

__quietly__()  { "$@" @@QUIET@@; }
__nostdout__() { "$@" @@NO_STDOUT@@; }
__nostderr__() { "$@" @@NO_STDERR@@; }
