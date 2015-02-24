#!/bin/sh
dodir "${D}/dev"

if [ "$(id -u)" != "0" ]; then
   # booting without static /dev nodes is known to fail for UEFI systems
   printf "%s\n" "uid != 0, cannot create static device nodes" 1>&2
else
   dochr console 5   1  0622

   # some additional devices nodes
   #  usually not required, uncomment to enable

   #dochr null    1   3  0666
   #dochr ttyS0   4  64  0660  0 5
   #dochr tty0    4   0  0622  0 5
   #dochr tty1    4   1  0622  0 5
   #dochr tty     5   0  0666  0 5
fi
