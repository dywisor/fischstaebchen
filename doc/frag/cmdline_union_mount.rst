The ``squashed_usr``, ``etc`` and ``volatile_rootfs``
cmdline parameters accept a number of comma-separated arguments:

   <file uri>, file=<file uri>
      Base squashfs file.

   auto
      Specifies that the base file is a squashfs file and can be found
      at ``<rootfs>/usr.squashfs`` or ``<rootfs>/usr.sfs``
      (+ ``.new``, ``.old`` file suffixes).

   nofile
      Disables any earlier base file.

   volatile, copy, import
      Copy all squashfs files to a tmpfs container and mount them from
      there.

   no_volatile, no_copy, no_import
      Disables ``copy``.

   rotate, rot
      When used with ``auto`` and a ``.new`` file is found:
      rotate the squashfs file as follows:

      * ``<file>.old => <file>.<date>``
      * ``<file> => <file>.old``
      * ``<file>.new => <file>``

   no_rotate
      Do not rotate the base file. This is the default behavior.

   overlay=<file uri>
      Adds an overlay squashfs file to the union mount.
      May be specified more than once.
      Passing an empty file (``overlay=``)
      disables all previously defined overlay files.

   no_overlay
      Disables all previously defined overlay files (same as ``overlay=``).

   tarball=<file uri>, tb=<file uri>
      Adds a tarball file to the union mount.

      Can be specified more than once, an empty file uri disables all
      previusoly defined tarball files.

      The tarballs are extracted to a tmpfs-backed layer
      on top of the squashfs layer and below the disk/mem layers.

   low_tarball=<file uri>, ltb=<file uri>
      Adds a low-layer tarball file to the union mount.

      Can be specified more than once, an empty file uri disables all
      previusoly defined low-layer tarball files.

      The tarballs are extracted to a tmpfs-backed layer
      below the squashfs layer.

   disk=<disk>
      Adds a disk to the union mount.
      Can be specified only once (overrides earlier declarations).

   disk_type=<fstype>
      Filesystem type of the disk. Defaults to "auto".

   disk_opts=<options>, disk_flags=<options>
      Additional mount options for the backing disk.

   no_disk
      Disables any earlier ``disk=`` parameter.

   mem[="zram"|"tmpfs"]
      Adds a backing memory branch (on top of all other layers).

      Can either be a zram block device or a tmpfs and defaults to zram.
      Note that zram falls back to tmpfs if no free zram device is available.

   mem_size=<size>, size=<size>
      Size of the backing memory branch, either as percentage or
      as size in Bytes (with the usual suffixes - ``k``, ``m``, ``g``).
      Defaults to 50%.

   no_mem
      Disables the backing memory branch.

   rw
      Adds a backing memory branch if no disk configured.

   cam
      Shortcut for ``copy``, ``auto``, ``mem=zram``.
