## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

_phaseout_zap_vars() {
   PHASEOUT_HOOK_NAMESPACE=
   PHASEOUT_HOOK_TMPDIR=
   PHASEOUT_HOOK_DIR=

   PHASEOUT_HOOK_SYNCDIR=
   PHASEOUT_HOOK_SYNCDIR_SUCCESS=
   PHASEOUT_HOOK_SYNCDIR_PENDING=
   PHASEOUT_HOOK_SYNCDIR_GRP=

   F_PHASEOUT_HOOK_FILTER=
   PHASEOUT_HOOK_MODE=
   __phaseout_tmpdir=
}

phaseout_init_env() {
   : ${PHASEOUT_FORCE_SINGLE:=n}
   : ${PHASEOUT_TMPDIR:=${ISHARE_TMP:?}/phaseout}

   _phaseout_zap_vars

   PHASEOUT_HOOK_NAMESPACE="${1:?}"
   PHASEOUT_HOOK_TMPDIR="${PHASEOUT_TMPDIR}/${1:?}-hook"

   PHASEOUT_HOOK_DIR="${PHASEOUT_HOOK_TMPDIR}/scripts"

   PHASEOUT_HOOK_SYNCDIR="${PHASEOUT_HOOK_TMPDIR}/status"
   PHASEOUT_HOOK_SYNCDIR_SUCCESS="${PHASEOUT_HOOK_SYNCDIR}/success"
   PHASEOUT_HOOK_SYNCDIR_PENDING="${PHASEOUT_HOOK_SYNCDIR}/fail"
   PHASEOUT_HOOK_SYNCDIR_GRP="${PHASEOUT_HOOK_SYNCDIR}/groups"
}

_phaseout_init_fs() {
   dodir "${PHASEOUT_HOOK_TMPDIR}"
   dodir "${PHASEOUT_HOOK_DIR}"
   dodir "${PHASEOUT_HOOK_SYNCDIR}"
   dodir "${PHASEOUT_HOOK_SYNCDIR_SUCCESS}"
   dodir "${PHASEOUT_HOOK_SYNCDIR_PENDING}"
   dodir "${PHASEOUT_HOOK_SYNCDIR_GRP}"
}

phaseout_init() {
   phaseout_init_env "${@}" && \
   _phaseout_init_fs
}

phaseout_cleanup_fs() {
   : "${PHASEOUT_HOOK_TMPDIR:?}"
   local iter

   for iter in \
      "${PHASEOUT_HOOK_DIR}" \
      "${PHASEOUT_HOOK_SYNCDIR_SUCCESS}" \
      "${PHASEOUT_HOOK_SYNCDIR_PENDING}" \
      "${PHASEOUT_HOOK_SYNCDIR_GRP}" \
      "${PHASEOUT_HOOK_SYNCDIR}"
   do
      case "${iter}" in
         "${PHASEOUT_HOOK_TMPDIR:?}/"*)
            veinfo "Removing ${iter}"
            autodie rm -r -- "${iter}"
         ;;
      esac
   done

   veinfo "Removing ${PHASEOUT_HOOK_TMPDIR}"
   autodie rm -r -- "${PHASEOUT_HOOK_TMPDIR}"
}

phaseout_finalize() {
   phaseout_cleanup_fs && \
   _phaseout_zap_vars
}
