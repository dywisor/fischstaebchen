create-user-tmpdirs <passwd file> <tmpdir root>
   Creates per-user directories in ``<tmpdir root>`` for all users read
   from ``<passwd file>``.

   Example Usage::

      create-user-tmpdirs /etc/passwd /var/tmp/users

filescan {-e|-f|-d|-b|-c} [--] <basepath> [<basepath>...] -- <suffix> [<suffix>...]
   Searches for the first ``<basepath><suffix>`` path that exists and
   prints it to stdout.

   The mode flags ``-e, -f, -d, -b, -c`` may be used to restrict the
   file type (exists, file, dir, block dev, char dev).
   The default mode is ``-f``.

   The ``.`` suffix has the special meaning of "match basepath".

   Example Usage::

      filescan -f -- /mnt/disk/* -- docs.tar

      filescan -f -d -- /etc -- machine-id


filesize <file>
   Reports the size of ``<file>`` in MiB.

pingloop [<host>]
   Repeatedly pings ``<host>``.

   Example Usage::

      pingloop 192.168.1.1

   Not part of the *init utils*.

ssh-validate-sync-command
   Executes the command read from the ``SSH_ORIGINAL_COMMAND`` environment
   variable if it does not contain "malicious" chars
   (``& ( ) { } ; < > ` |``) and is a valid sync command:

      * ``scp -t ...``

      * ``rsync --server ...``

      * ``mkdir ...``

   This program cannot be used directly and should be set up in
   the ``.ssh/authorized_keys`` file (on the server side)::

      command="/usr/local/bin/ssh-validate-sync-command" ssh-rsa <key>...

   Not part of the *init utils*.

untar [-C <dir>] <tarball>
   Uncompresses and unpacks a tarball to ``<dir>``
   (or the current working directory).
   ``<dir>`` gets automatically created if necessary.
   The tarball compression is detected based on the file suffix.

   This is similar to ``mkdir -p <dir> && tar xap -C <dir> -f <tarball>``,
   but deals with ``lzo`` and ``lz4`` compressed tarballs
   (which is not supported by busybox' tar).

zram-autoswap [<size> [<num swaps>]]
   Creates up to ``<num swaps>`` zram swap devices that do no exceed ``<size>``
   in total.

   ``<num swaps>`` default to the cpu core count and the default``<size>``
   depends on the amount of system memory.
   See the ``zram_swap`` boot option for details.

   **This program is not available**, but trivial to implement.
   It's all there - see ``init/src/zram/autoswap.h, zram_autoswap()``.

ztmpfs [-h] [-f] [-q] [-t <type>] [-o <opts>] [-S <size>] [-m <mode>] [<name>] <mountpoint>
   Allocates a zram device, formats it as ext2/ext4/btrfs filesystem and
   mounts it on  ``<mountpoint>``.
   Falls back to tmpfs mounts if no free zram device is available.

   Options:

      -h
         Print help.

      -f
         Fake mode - print what would be done.

      -q
         Be quiet.

      -t <type>
         Takes on arg, either ``ztmpfs`` or ``zdisk``.
         This controls the default mode of the zram mount
         (``1777`` for ztmpfs, ``0755`` for zdisk).

         The type is usually derived from the program's basename.

      -o <opts>
         May be used to pass arbitrary mount options to the mount command.
         The mount options must be supported by both ``mount -t tmpfs`` and
         ``mount -t ext[24]``.
         (``size=`` and ``mode=`` options get filtered out.)

      -S <size>
         Size of the zram disk. Takes precedence over ``-o size=<size>``.

         Defaults to ``50%``.

      -m <mode>
         Mount mode. Takes precedence over ``-o mode=<mode>`` and the
         default mode of ``<type>``.

   Positional Arguments:

      <name>
         Filesystem name, also used as ``LABEL`` when formatting a zram blockdev.
         Defaults to ``zram<N>`` or ``tmpfs``.

      <mountpoint>
         Mountpoint, will be created if necessary.
         This option is **mandatory**.
