## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

stagedive_inherit  base


stagedive_want  baselayout/mkuser
stagedive_want  localegen


ishare_add_flag stagedive-bootstrap-want-create-user
STAGEDIVE_MKUSER_NAME=live
STAGEDIVE_MKUSER_PASS=live
STAGEDIVE_MKUSER_GROUPS=users
