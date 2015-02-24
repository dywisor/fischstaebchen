## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

#!/bin/sh

int get_metascript_file ( name, **v0! ) {
   v0="${METASCRIPT_DIR}/${1:?}.mscript"
   [ -f "${v0}" ]
}

@stdout int do_gen_metascript ( *var_spec, **mscript ) {
   <%%varcheck mscript%>
   <%%locals vname val%>

   set -- "$@" --

   while [ "${1:---}" != "--" ]; do
      case "${1}" in
         '_'|'.')
            @@NOP@@
         ;;

         ?*=*)
            vname=@@KEYOF1@@
            val=@@VALOF1@@

            set -- "$@" -e "s=(^|[^@])@${vname}@=\1${val}=g"
         ;;

         *)
            die "do_gen_metascript(): invalid arg: ${1}"
         ;;
      esac

      shift
   done

   shift || die

   < "${mscript}" sed -r -e '/^\s*[#][#]/d' "$@" -e 's=@@([^@]|$)=@\1=g'
}

void print_gen_from_metascript ( template_name, *var_spec ) {
   <%%locals mscript v0%>

   autodie get_metascript_file "${1}"
   <%%v0 mscript%>

   shift || die
   [ "${1:-X}" != "--" ] || shift || die

   do_gen_metascript "$@"
}

void gen_from_metascript (
   template_name, phase, script_name, *var_spec, **v0!
) {
   <%%varcheck ISHARE_HOOKS 1..3 %>
   <%%locals mscript mscript_out phase hdir%>

   autodie get_metascript_file "${1}"
   <%%v0 mscript%>

   case "${2}" in
      pre|post)   phase="${2}-mount-newroot" ;;
      _)          phase="mount-newroot" ;;
      *)          phase="${2}" ;;
   esac

   hdir="${ISHARE_HOOKS}/${phase}.d"
   mscript_out="${hdir}/${3%.sh}.sh"

   autodie mkdir -p -- "${hdir}"
   [ -h "${ISHARE_HOOKS}/${phase}" ] || \
      autodie ln -s -- "${IHOOK}/_stdmux.sh" "${ISHARE_HOOKS}/${phase}"

   shift 3 || die
   [ "${1:-X}" != "--" ] || shift || die

   do_gen_metascript "$@" > "${mscript_out}" || die
   v0="${mscript_out}"
}
