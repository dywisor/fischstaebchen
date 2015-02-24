## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

list_has() {
   <%%locals word< %>

   <%%argc_loop ::: [ "${1}" != "${word}" ] || return 0 %>
   return 1
}
