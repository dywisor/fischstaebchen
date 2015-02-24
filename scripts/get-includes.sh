#!/bin/bash
# super hacky script for getting a list of required objects/headers
#
#  Usage: get-includes <entry point>
#
#  Hint:
#  * cd into the src dir before calling this script.
#  * use sort:   get-includes <entry point> | sort     [-k 1,1 -k 2,2]
#
Q=\'
QQ=\"

case "${1-}" in
   '--prefix'|'--xprefix'|'-x')
         XPREFIX="${2:?}"
         shift 2 || exit
   ;;
esac


declare -A _INCLUDES=()

die() {
   printf "%s\n" "${1:+died: }${1:-died.}" 1>&2
   exit ${2:-2}
}

_read_includes() {
   sed -n -r -e "s@^[#]include\s+${QQ}([^${QQ}]+)${QQ}.*\$@\1@p" < "${1}"
}

_normpath() {
   python -c '\
from __future__ import print_function;
import os.path;
import sys;

print ( os.path.normpath ( sys.argv[1] ) );' "${1}"
}

check_have_include() {
   if [ -n "${_INCLUDES[${1}]+SET}" ]; then
      return 0
   fi

   _INCLUDES[${1}]=YES
   return 1
}

if [ -n "${XPREFIX=}" ]; then
print_include() {
   local t
   case "${1:?}" in
      O) t="OBJECTS" ;;
      H) t="HEADERS" ;;
      A) t="ANY" ;;
   esac
   printf "%s_%-15s += %s\n" "${XPREFIX}" "${t}_NAMES" "${2:?}"
}
else
print_include() {
   printf "%s %s\n" "${1:?}" "${2:?}"
}
fi

expand_includes() {
   local f
   local d
   local npath
   local cfile
   local hfile
   local itype

   f="${1:?}"; f="${f%.[ch]}"
   cfile="${f}.c"
   hfile="${f}.h"
   test -f "${cfile}" || cfile=
   test -f "${hfile}" || hfile=

   [ -n "${cfile}${hfile}" ] || die "no such .c/.h file: ${f}._"

   npath="$( _normpath "${f}" )"
   [ -n "${npath}" ] || die "failed to get normpath"

   if check_have_include "${npath}"; then
      return 0
   fi

   d="$(dirname "${npath}")"
   [ -n "${d}" ] || die "failed to get dirname"

   set -- $( _read_includes ${cfile} ${hfile} ) || \
      die "failed to read includes: ${cfile} ${hfile}"


   if [ -n "${cfile}" ]; then
      if [ -n "${hfile}" ]; then
         itype=A
      else
         itype=O
      fi
   else
      itype=H
   fi

   print_include "${itype}" "${npath}"

   while [ $# -gt 0 ]; do
      expand_includes "${d}/${1}" || return
      shift
   done

   return 0
}



while [ $# -gt 0 ]; do
   expand_includes "${1}"
   shift
done
