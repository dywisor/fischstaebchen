## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

stagedive_bootstrap_load_config() {
   <%%zapvars -p bootstrap_ path name uri size md5 sha1 sha256 sha512 %>

   loadscript_or_die "${STAGEDIVE_BOOTSTRAP_CFG:?}"

   <%% foreach_static n !\
      | name uri ::: !\
         |{I}[ -n "${{bootstrap_{n}-}}" ] || die "bootstrap_{n} is not set." !\
   %>

}

stagedive_bootstrap_print_banner() {
   if [ -n "${1-}" ]; then
      einfo "${1}" stagedive-bootstrap
   else
      einfo "Initializing a ${STAGEDIVE_TYPE} system" stagedive-bootstrap
   fi

   einfo "Using file ${bootstrap_name}" stagedive-bootstrap
   einfo "From ${bootstrap_uri}" stagedive-bootstrap
}

stagedive_bootstrap_default_init() {
   stagedive_bootstrap_load_config
   stagedive_bootstrap_print_banner "${@}"
}
