## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

rel_proot="/portage"
proot="${ROOT%/}${rel_proot}"


dodir "${proot}/tree/gentoo"
dodir "${proot}/distfiles"
dodir "${proot}/packages"

dodir "${proot}/local/packages"

fs_remove "${ROOT%/}/usr/portage"
autodie ln -s -- "${rel_proot}/tree/gentoo" "${ROOT%/}/usr/portage"

dodir \
   "${proot}/local/overlay/metadata" \
   "${proot}/local/overlay/profiles"

printf 'masters = %s\n' "gentoo" \
   > "${proot}/local/overlay/metadata/layout.conf" || die

printf '%s\n' "stage_local" \
   > "${proot}/local/overlay/profiles/repo_name" || die
