## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## _phaseout_install_local_as ( hook_srcroot, src_name, dst_name )
##
_phaseout_install_local_as() {
   : ${PHASEOUT_HOOK_NAMESPACE:?}
   : ${PHASEOUT_HOOK_DIR:?}
   <%%varcheck 1..3 %>
   <%%locals hookf %>

   hookf="${1%/}/${2}"
   if [ -f "${hookf}" ]; then
      _phaseout_install_local__doins "${hookf}" "${3}"
   else
      die "missing ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${2} (${hookf})"
   fi
}

## _phaseout_install_local ( hook_srcroot, ["--"|"-r"|"-R"|"--recursive"], *name )
_phaseout_install_local() {
   : ${PHASEOUT_HOOK_NAMESPACE:?}
   : ${PHASEOUT_HOOK_DIR:?}
   <%%locals recursive src_root  %>

   src_root="${1:?}"; shift
   recursive=
   case "${1-}" in
      '-r'|'-R'|'--recursive')
         recursive=y
         shift || die
      ;;
      '--')
         shift || die
      ;;
   esac

   while [ ${#} -gt 0 ]; do
      autodie _phaseout_install_local__inner "${1:?}" "${1:?}"
      shift
   done
}

## _phaseout_install_local__doins ( src_file, dst_name_in )
##
_phaseout_install_local__doins() {
   <%%varcheck 1..2 %>
   <%%locals v0 %>

   <%% v0_strfeed ${2} sed -r -e 's=[/]+=-=g' %>
   autodie ln -f -s -- "${1}" "${PHASEOUT_HOOK_DIR:?}/${v0}.sh"
}

## _phaseout_install_local__inner ( src_name, dst_name, **src_root, **recursive )
##
_phaseout_install_local__inner() {
   <%%locals any_hook hookd hookf iter name %>

   [ "${1:-/}" != "/" ] || return @@EX_USAGE@@
   src_name="${1}"
   dst_name="${2:?}"
   set --

   any_hook=
   hookd="${src_root}/${src_name}"
   hookf="${hookd}.sh"

   if [ -f "${hookf}" ]; then
      _phaseout_install_local__doins "${hookf}" "${dst_name}"
      any_hook=y
   fi

   if [ -d "${hookd}" ]; then
      for iter in "${hookd}/"*.sh; do
         if [ -f "${iter}" ]; then
            name="${iter##*/}"; name="${name%.sh}"
            _phaseout_install_local__doins "${iter}" "${dst_name}/${name}"
            any_hook=y
         fi
      done

      if [ -n "${recursive}" ]; then
         for iter in "${hookd}/"*; do
            if [ -d "${iter}" ]; then
               name="${iter##*/}"
               _phaseout_install_local__inner \
                  "${src_name}/${name}" "${dst_name}/${name}"
               any_hook=y
            fi
         done
      fi
   fi

   if [ -z "${any_hook}" ]; then
      die "missing ${PHASEOUT_HOOK_NAMESPACE:-???} hook: ${src_name} (${hookf})"
   fi
}
