## Copyright (c) 2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##


parse_misc_kernel_modules_init() {
   ishare_add_flag want-xfer_kmod
   ishare_add_flag want-xfer_fw
}

parse_misc_kernel_modules() {
   case "${key}" in
      xfer_kmod|xfer_fw)
         if cmdline_value_bool_default_true; then
            ishare_add_flag "want-${key}"
         else
            ishare_del_flag "want-${key}"
         fi

         return 0
      ;;
   esac

   return 1
}

parse_misc_kernel_modules_done() {
   return 0
}

add_parser_with_init misc_kernel_modules
