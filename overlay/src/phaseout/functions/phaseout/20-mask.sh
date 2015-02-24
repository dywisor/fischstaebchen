## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

phaseout_apply_mask() {
   have_any_hook=
   <%%locals mask_iter file_iter name is_masked did_remove_all_hooks %>
   [ -n "${PHASEOUT_HOOK_DIR-}" ] || die "PHASEOUT_HOOK_DIR is not set."

   set -f; set -- ${*}; set +f;
   [ ${#} -gt 0 ] || return @@EX_USAGE@@

   for mask_iter; do
      case "${mask_iter}" in
         all|ALL)
            die "BUG: hook mask == all must be handled prior to calling phaseout_apply_mask()."
         ;;

         none|NONE)
            if [ ${#} -eq 1 ]; then
               have_any_hook=
               for file_iter in "${PHASEOUT_HOOK_DIR}/"*".sh"; do
                  if [ -f "${file_iter}" ]; then
                     have_any_hook=y
                     break
                  fi
               done
               return 0
            else
               eerror "Invalid hook mask: excepted list (x)or 'none', got both."
               return @@EX_USAGE@@
            fi
         ;;
      esac
   done


   have_any_hook=
   did_remove_all_hooks=
   for file_iter in "${PHASEOUT_HOOK_DIR}/"*".sh"; do
      [ -f "${file_iter}" ] || continue
      name="${file_iter##*/}"; name="${name%.sh}"

      is_masked=
      for mask_iter; do
         case "${name}" in
            ${mask_iter})
               is_masked="${mask_iter}"
               break
            ;;
         esac
      done

      if [ -n "${is_masked}" ]; then
         einfo "Removing masked hook: ${name}"
         veinfo "matched by: '${is_masked}'"

         autodie rm -- "${file_iter}"
         : ${did_remove_all_hooks:=y}
      else
         did_remove_all_hooks=n
         have_any_hook=y
      fi
   done

   if [ "${did_remove_all_hooks:-X}" = "y" ]; then
      ewarn "phaseout: ${PHASEOUT_HOOK_NAMESPACE:-???}: all hooks disabled."
      return 1
   fi

   return 0
}
