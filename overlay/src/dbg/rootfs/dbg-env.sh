#!/bin/sh
. "@@FUNCTIONS@@" || exit

sush() {
   (
      "$@" && rc=$? || rc=$?

      printf "%s\n" "rc=${rc}, v0=\"${v0:-%empty%}\""

      return ${rc}
   )
}
