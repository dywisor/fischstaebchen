## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

strfeed_cmd() {
   <%%locals word<=${1?} %>

   <%%strfeed "${word}" "$@"%>
}
