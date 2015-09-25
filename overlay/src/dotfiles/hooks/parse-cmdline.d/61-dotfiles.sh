## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

parse_dotfiles__zap() {
   CMDLINE_DOTFILES_TBT_SRC_URI=

   {
      :> "${DOTFILES_SRC_LIST_FILE:?}.in.file" && \
      :> "${DOTFILES_SRC_LIST_FILE:?}.in.git"
   } || die "Failed to empty dotfiles src list!"
}

parse_dotfiles__zap

parse_dotfiles_done() {
   add_env CMDLINE_DOTFILES_TBT_SRC_URI "${CMDLINE_DOTFILES_TBT_SRC_URI-}"

   return 0
}

parse_dotfiles() {
   case "${key}" in
      dotfiles_tbt|dcfg_tbt)
         CMDLINE_DOTFILES_TBT_SRC_URI="${value}"
         return 0
      ;;

      dotfiles|dfil|dotconfig|dcfg)
         if [ "${have_value}" != "y" ]; then
            @@NOP@@

         elif [ -z "${value}" ]; then
            parse_dotfiles__zap

         elif preparse_file_uri_verbose "${value}"; then
            case "${type}" in
               git)
                  printf '%s\n' "${uri}" >> "${DOTFILES_SRC_LIST_FILE}.in.git" || \
                     die "Failed to write dotfiles src list!"
               ;;
               *)
                  printf '%s\n' "${uri}" >> "${DOTFILES_SRC_LIST_FILE}.in.file" || \
                     die "Failed to write dotfiles src list!"
               ;;
            esac

         else
            die "Could not parse file uri: ${value}"
         fi

         return 0
      ;;
   esac

   return 1
}

add_parser dotfiles
