## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

root_set_fspaths() {
   : ${ROOT:?}

   : ${RREL_LIB_SYSTEMD:=/usr/lib/systemd}
   RREL_LIB_SYSTEMD_SYSTEM="${RREL_LIB_SYSTEMD}/system"

   : ${RREL_ETC:=/etc}

   : ${RREL_ETC_UDEV:=${RREL_ETC}/udev}
   RREL_ETC_UDEV_RULES="${RREL_ETC_UDEV}/rules.d"

   : ${RREL_ETC_SYSTEMD:=${RREL_ETC}/systemd}
   RREL_ETC_SYSTEMD_SYSTEM="${RREL_ETC_SYSTEMD}/system"

   : ${RREL_ETC_CONFD:=${RREL_ETC}/conf.d}
   : ${RREL_ETC_INITD:=${RREL_ETC}/init.d}

   _root_set_abspaths
}

_root_set_abspaths() {
   <%% static_foreach name !\
      | LIB_SYSTEMD !\
      | LIB_SYSTEMD_SYSTEM !\
      !\
      | ETC_UDEV !\
      | ETC_UDEV_RULES !\
      !\
      | ETC_SYSTEMD !\
      | ETC_SYSTEMD_SYSTEM !\
      !\
      | ETC_CONFD !\
      | ETC_INITD !\
      !\
      | ::: !\
      !\
      |{NL} !\
      |{I}case "${{RREL_{name}?}}" in{NL} !\
      |{I}   ''|_*) ROOT_{name}="" ;;{NL} !\
      |{I}   *) ROOT_{name}="${{ROOT%/}}/${{RREL_{name}#/}}" ;;{NL} !\
      |{I}esac{NL} !\
      |{I}R_{name}="${{ROOT_{name}}}" !\
   %>

}

root_detect_fspaths() {
   local iter

   if \
      test_fs_is_real_dir  "${ROOT%/}/usr/lib64" || \
      test_fs_is_real_dir  "${ROOT%/}/lib64"
   then
      ROOT_LIBDIR_NAME=lib64
   else
      ROOT_LIBDIR_NAME=lib
   fi

   RREL_LIB_SYSTEMD=
   for iter in \
      "/usr/lib/systemd" \
      "/lib/systemd" \
      \
      "/usr/${ROOT_LIBDIR_NAME}/systemd" \
      "/${ROOT_LIBDIR_NAME}/systemd"
   do
      if [ -d "${ROOT%/}/${iter#/}" ]; then
         RREL_LIB_SYSTEMD="${iter}"
         break
      fi
   done

   root_set_fspaths "${@}"
}

check_fspath_configured() {
   while [ ${#} -gt 0 ]; do
      case "${1}" in
         /*) return 0 ;;
      esac
      @@SHIFT_OR_RET@@
   done

   return 1
}
