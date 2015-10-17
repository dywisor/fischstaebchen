## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

parse_storage_layers__use_bcache=y
parse_storage_layers__use_mdadm=y
parse_storage_layers__use_lvm=y

parse_storage_layers() {
   case "${key}" in
      dobcache) parse_storage_layers__use_bcache=y ;;
      nobcache) parse_storage_layers__use_bcache=n ;;

      dolvm)    parse_storage_layers__use_lvm=y ;;
      nolvm)    parse_storage_layers__use_lvm=n ;;

      domdadm)  parse_storage_layers__use_mdadm=y ;;
      nomdadm)  parse_storage_layers__use_mdadm=n ;;

      *)
         return 1
      ;;
   esac

   return 1
}

parse_storage_layers_done() {
   autodie ishare_set_flag \
      storage-use-bcache "${parse_storage_layers__use_bcache}"

   autodie ishare_set_flag \
      storage-use-mdadm "${parse_storage_layers__use_mdadm}"

   autodie ishare_set_flag \
      storage-use-lvm "${parse_storage_layers__use_lvm}"
}

add_parser storage_layers
