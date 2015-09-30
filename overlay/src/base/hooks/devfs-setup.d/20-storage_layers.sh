## Copyright (c) 2014-2015 André Erdmann <dywi@mailerd.de>
##
## Distributed under the terms of the MIT license.
## (See LICENSE.MIT or http://opensource.org/licenses/MIT)
##

have_bcache=n
if ishare_has_flag storage-use-bcache; then
    if [ -e /sys/fs/bcache/register_quiet ]; then
        have_bcache=y
    fi
fi

have_mdadm=n
if ishare_has_flag storage-use-mdadm; then
    if [ -e /proc/mdstat ]; then
        have_mdadm=y
    fi
fi

have_lvm=n
if ishare_has_flag storage-use-lvm; then
    if \
        [ -e /sys/class/misc/device-mapper ] && \
        [ -e /dev/mapper/control ]
    then
        have_lvm=y
    fi
fi



_scan_bcache() {
    <%%locals major minor blocks name BOGUS %>

    [ -r /proc/partitions ] || return 1

    while read -r major minor blocks name BOGUS; do
        if \
            [ -z "${BOGUS-}" ] && \
            [ "${name:-name}" != "name" ] && \
            [ -b "/dev/${name}" ]
        then
            printf '%s\n' "/dev/${name}" \
                > /sys/fs/bcache/register_quiet || @@NOP@@
        fi
    done < /proc/partitions
}

scan_bcache() {
    [ "${have_bcache}" = "y" ] || return 0
    if _scan_bcache "${@}"; then
        return 0
    else
        ewarn "Failed to scan for bcache devices!"
        return 1
    fi
}


scan_bcache

if [ "${have_mdadm}" = "y" ]; then
    einfo "Scanning for software raid arrays"

    if [ ! -f /etc/mdadm.conf ]; then
        mdadm --examine --scan > /etc/mdadm.conf || \
            die "Failed to create /etc/mdadm.conf!"
    fi

    if retlatch \
        mdadm --assemble --scan
    then
        @@NOP@@
    elif [ ${rc} -eq 1 ]; then
        ## mdadm returns 1 in case of "no arrays found"
        @@NOP@@

    else
        ewarn "Failed to scan for software raid arrays! (rc=${rc})"
    fi

    scan_bcache
fi

if [ "${have_lvm}" = "y" ]; then
    if \
        dodir_nonfatal /etc/lvm/cache && \
        3>&- vgscan --mknodes
    then
        einfo "Scanning for volume groups"

        vgchange -a -y || \
            ewarn "Failed to scan for volume groups!"
    else
        ewarn "Failed to initialize lvm"
    fi

    scan_bcache
fi
