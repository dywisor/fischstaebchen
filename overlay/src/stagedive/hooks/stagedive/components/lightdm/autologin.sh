## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

autologin_data="\
pam-service=lightdm-autologin
user-session=${LIGHTDM_AUTOLOGIN_SESSION:?}
autologin-user=${LIGHTDM_AUTOLOGIN_USER:?}
autologin-user-timeout=0"


f="${ROOT%/}/etc/lightdm/lightdm.conf"
autodie test -f "${f}"
autodie rm -f -- "${f}.tmp"
autodie mv -f -- "${f}" "${f}.dist"

## __edit_lightdm_config_append ( section, data )
__edit_lightdm_config_append() {
awk -v data="${2:?}" -v section="[${1:?}]" \
'
BEGIN { m=0; }

{ blind=0; }
( (!m) && ($1 == ("#" section)) )  { $1 = ""section; }
( (!m) && ($1 == (""  section)) )  { m=1; blind=1; }


( (m == 1) && (!blind) && ($0 ~ "^[#]?\\[.*\\]") ) {
   printf("\n%s\n\n",data);
   m = 2;
}

{ print; }

END { if (!m) { printf("\n%s\n%s\n",""section,""data); } }
'
}

## edit_lightdm_config_append ( srcfile, dstfile, section, data )
edit_lightdm_config_append() {
   __edit_lightdm_config_append \
      "${3:?}" "${4:?}" < "${1:?}" > "${2:?}" || \
         die "Failed to edit/create lightdm.conf!"
}

## __edit_lightdm_config_set ( section, key, value )
__edit_lightdm_config_set() {
awk -F = -v section="[${1:?}]" -v key="${2:?}" -v value="${3?}" \
'
BEGIN { m=0; }

{ blind=0; no_print=0; }
( (!m) && ($1 == ("#" section)) )  { $1 = ""section; }
( (!m) && ($1 == (""  section)) )  { m=1; blind=1; }

( ((m == 1) || (m == 2)) && ($1 == (""key)) && (!blind) ) {
   printf("\n%s=%s\n\n",""key,""value);
   no_print=1; m=2;
}

( (m == 1) && (!blind) && ($0 ~ "^[#]?\\[.*\\]") ) {
   printf("\n%s=%s\n\n",""key,""value);
   m = 3;
}

(!no_print) { print; }

END { if (!m) { printf("\n%s\n%s=%s\n",""section,""key,""value); } }'
}

## edit_lightdm_config_set ( srcfile, dstfile, section, key, value )
edit_lightdm_config_set() {
   __edit_lightdm_config_set \
      "${3:?}" "${4:?}" "${5?}" < "${1:?}" > "${2:?}" || \
         die "Failed to edit key/value in lightdm.conf!"
}

edit_lightdm_config_append "${f}.dist" "${f}" \
   "SeatDefaults" "${autologin_data}"

##edit_lightdm_config_set "${f}.tmp" "${f}" \
##   "LightDM" "minimum-vt" "7"
##
##autodie rm -- "${f}.tmp"
