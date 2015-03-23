## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

defparser_rootfstype=
CMDLINE_ROOTPASS=

: ${APT_PROXY=}

parse_default() {
   case "${key}" in
      inet)
         ishare_set_flag want-net "${value:-y}"
      ;;

      keep-net)
         ishare_set_flag keep-net "${value:-y}"
      ;;

      rootfstype)
         defparser_rootfstype="${value}"
      ;;

      rootpass)
         CMDLINE_ROOTPASS="${value}"
      ;;

      apt_cacher_ng|apt_proxy|aproxy)
         APT_PROXY="${value}"
      ;;

      *)
         return 1
      ;;
   esac

   return 0
}

parse_default_done() {
   if check_is_keepval "${APT_PROXY=}"; then
      APT_PROXY=none
      APT_PROXY_PREFIX=
   else
      APT_PROXY_PREFIX="${APT_PROXY%/}/"
   fi

   case "${defparser_rootfstype-}" in
      nfs|cifs)
         veinfo "Automatically enabling want-net flag due to rootfstype=${defparser_rootfstype}"
         ishare_add_flag want-net

         veinfo "Automatically enabling keep-net flag due to rootfstype=${defparser_rootfstype}"
         ishare_add_flag keep-net
      ;;
   esac

   add_env_var \
      CMDLINE_ROOTPASS    \
      APT_PROXY            \
      APT_PROXY_PREFIX

   return 0
}

add_parser default
