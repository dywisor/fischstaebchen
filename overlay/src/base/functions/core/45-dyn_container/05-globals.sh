## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

## int _dyncontainer_calculate_initial_container_size()
##
_dyncontainer_calculate_initial_container_size() {
   <%%locals -i tenth csize %>

   if ! { get_sysmem && [ ${SYSMEM:-0} -gt 0 ]; }; then
      eerror \
         "SYSMEM is not set - cannot calculate default dyn container size!"
      return 1
   fi

   <%%div tenth=SYSMEM 10 %>
   if [ ${tenth} -lt 1 ]; then
      eerror \
         "system memory is too small - cannot calculate default dyn container size!"
      return 1
   fi

   # set default dyn container size to <=70%
   <%%mult csize=tenth 7 %>
   if [ ${csize} -le ${tenth} ]; then
      # overflow (and/or bad input)
      eerror "failed to calculate default dyn container size!"
      return 2
   fi

   if [ "${_DYNCONTAINER_INIT_SIZE:-X}" != "${csize}" ]; then
      # yeah, that's a str check

      _DYNCONTAINER_INIT_SIZE="${csize}"
      add_env _DYNCONTAINER_INIT_SIZE "${csize}"
   fi
}

dyncontainer_init_globals() {
   if [ -z "${_DYNCONTAINER_INIT_SIZE-}" ]; then
      autodie _dyncontainer_calculate_initial_container_size
   fi
}
