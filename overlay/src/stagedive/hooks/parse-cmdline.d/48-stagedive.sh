## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

STAGEDIVE_TYPE=
STAGEDIVE_AUTOSET_FILE_CFG=

: ${STAGEDIVE_HOOK_MASK=}
STAGEDIVE_CFG_KEYMAP=
STAGEDIVE_CFG_TIMEZONE=


parse_stagedive_done() {
   : ${STAGEDIVE_TYPE:=none}
   : ${STAGEDIVE_HOOK_MASK:=none}
   : ${STAGEDIVE_AUTOSET_FILE_CFG:=none}

   : ${STAGEDIVE_CFG_KEYMAP:=de-latin1-nodeadkeys}
   : ${STAGEDIVE_CFG_TIMEZONE:=Europe/Berlin}

   <%% foreach_static var  !\
      | TYPE               !\
      | HOOK_MASK          !\
      | AUTOSET_FILE_CFG   !\
      !\
      | CFG_KEYMAP         !\
      | CFG_TIMEZONE       !\
      !\
      | ::: !\
      !\
      |{NL} !\
      |{I}if check_is_keepval "${{STAGEDIVE_{var}?}}"; then{NL} !\
      |{I}   STAGEDIVE_{var}=none{NL} !\
      |{I}fi !\
   %>


   add_env_var \
      STAGEDIVE_TYPE               \
      STAGEDIVE_HOOK_MASK          \
      \
      STAGEDIVE_AUTOSET_FILE_CFG   \
      \
      STAGEDIVE_CFG_KEYMAP         \
      STAGEDIVE_CFG_TIMEZONE

   return 0
}


parse_stagedive__stagedive() {
   <%%locals stage_type arg stage_file is_file_uri is_base_uri %>

   case "${value}" in
      *,*)
         stage_type="${value#*,}"
         arg="${value%%,*}"
      ;;
      */*)
         stage_type=gentoo-stage3
         arg="${value}"
      ;;
      *)
         stage_type="${value}"
         arg=
      ;;
   esac

   case "${stage_type}" in
      default|_)
         stage_type=default
      ;;
      gentoo)
         stage_type=gentoo
      ;;
      g)
         stage_type=gentoo-stage3
      ;;
      G)
         stage_type=gentoo-stage4
      ;;
      stage[0-9])
         stage_type=gentoo-${stage_type}
      ;;
   esac

   if [ ! -f "${STAGEDIVE_PROFILES_DIR:?}/${stage_type}.sh" ]; then
      die "Unknown stagedive type: ${stage_type}!"
   fi

   STAGEDIVE_AUTOSET_FILE_CFG=
   STAGEDIVE_TYPE="${stage_type}"

   is_base_uri=
   is_file_uri=
   stage_file=

   case "${arg}" in
      none)  is_file_uri=y ;;
      ?*/)   is_base_uri=y ;;
      ?*/?*) is_file_uri=y; stage_file="${arg}" ;;
   esac

   if [ -n "${is_file_uri}" ]; then
      @@NOP@@

   elif [ -n "${is_base_uri}" ]; then

      case "${stage_type}" in
         gentoo-stage*)
            stage_file="tarball=${arg}/${stage_type#gentoo-}-${ALT_ARCH}.tar.bz2"
         ;;

         *)
            die "parse_cmdline: stagedive:${stage_type} does not support base uri(s)!"
         ;;
      esac

   else
      stage_file="@bootstrap"

      case "${stage_type}" in
         gentoo-stage3)
            case "${arg}" in
               *:*)
                  # <pri_arch>:<sub_arch>[-<variant>]
                  STAGEDIVE_AUTOSET_FILE_CFG="${arg}"
               ;;

               *)
                  # [<variant>]
                  STAGEDIVE_AUTOSET_FILE_CFG="${ARCH}:${ALT_ARCH}${arg:+-${arg}}"
               ;;
            esac
         ;;

         archstrap*)
            STAGEDIVE_AUTOSET_FILE_CFG="${arg:-${ARCH}}"
         ;;

         *)
            die "parse_cmdline: stagedive:${stage_type} needs a file uri!"
         ;;
      esac
   fi

   arg=
   case "${stage_file}" in
      '')
         @@NOP@@
      ;;

      '@bootstrap')
         ## dummy arg
         arg="yes"
      ;;

      squashfs=*|sfs=*)
         arg="file=${stage_file#*=}"
      ;;

      tarball=*|tb=*)
         arg="low_tarball=${stage_file#*=}"
      ;;

      *.squashfs|*.sfs)
         arg="file=${stage_file}"
      ;;

      *)
         arg="low_tarball=${stage_file}"
      ;;
   esac

   if [ -n "${arg}" ]; then
      autodie do_parse_cmdline \
         "volatile_rootfs=no_mem,no_file,no_overlay,no_tarball,no_low_tarball,${arg}"
   fi
}

parse_stagedive() {
   case "${key}" in

      stagedive)
         parse_stagedive__stagedive
         return 0
      ;;

      stagedive_mask)
         if [ -z "${value}" ]; then
            STAGEDIVE_HOOK_MASK=

         elif [ -z "${STAGEDIVE_HOOK_MASK-}" ]; then
            STAGEDIVE_HOOK_MASK="${value_components:?}"

         else
            STAGEDIVE_HOOK_MASK="${STAGEDIVE_HOOK_MASK} ${value_components:?}"

         fi

         return 0
      ;;

      setkmap)
         STAGEDIVE_CFG_KEYMAP="${value}"
         return 0
      ;;

      timezone|tz)
         STAGEDIVE_CFG_TIMEZONE="${value}"
         return 0
      ;;

   esac

   return 1
}


add_parser stagedive
