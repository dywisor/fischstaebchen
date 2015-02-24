quiet
   Disables most messages (affects both kernel and */init*).

   (*/init* parses only kernel verbosity options that disable output,
   i.e. *quiet*, but not *debug*)

iquiet
   Disables most messages.

noquiet
   Disables *iquiet*/*quiet*.

iverbose
   Enables verbose messages.

idebug
   Enables debug messages.

console=<tty>...
   Colored output gets automatically disabled if ``<tty>`` starts with
   ``ttyS``, ``ttyAMA`` or ``ttyMXC``.

modprobe=<module>[,<module>...]
   Comma-separated list of kernel modules to be loaded.
   May be specified more than once.

load_modules=<module>[,<module>...]
   Alias to ``modprobe=``.

real_init=<path>
   Program to be executed when switching to */newroot*.
   Gets autodetected by default.

init=<path>
   Alias to ``real_init=``.

ro
   Specifies that */newroot* should be mounted readonly.
   This is the default.

rw
   Specifies that */newroot* should be mounted in read-write mode.

norootmount
   Specifies that */init* should not try to mount */newroot*.
   A hook should take care of this instead.

root=<fs>
   Root filesystem, may be any of:

   * device path, e.g. */dev/sda2*
   * disk by label/uuid, e.g. *LABEL=rootfs*
   * disk by partition uuid (understood, but not supported)
   * nfs, e.g. "10.10.10.1:/this/machine/rootfs"
   * probably also a cifs share path (not tested)
   * any name, see ``rootfstype`` below.

   Overrides any earlier ``root`` parameter.

rootfstype=<fstype>
   Root filesystem type, defaults to "auto".
   In addition to the usual types, ``zram`` and ``zdisk`` are also supported.

   Overrides any earlier ``rootfstype`` parameter.

rootfsflags=[<fsflags>]
   Mount options for */newroot*.

   Overrides any earlier ``rootfsflags`` parameter.

rootfind=[timeout_centisecs]
   Specifies the timeout in centiseconds when waiting for disks.

   A value of less than 0 or greater than 10000 sets the default duration (5s),
   0 disables waiting.

   Overrides any earlier ``rootfind`` parameter.

nousrmount
   Specifies that */init* should not try to mount */newroot/usr*.
   A hook should may take care of this instead.

   Note that it is not necessary to specify this parameter when using hooks
   that mount */newroot/usr*.
   These hooks should "talk back" to */init* instead.

squashed_usr=<file>
   Specifies a file that should be mounted on */newroot/usr*.

   <file> should be an absolute path relative to */newroot*,
   e.g. */usr.squashfs*.

   Note that */init* provides basic squashfs-mount support only.
   Use the the union_mount[_base] overlays for writable mounts
   with backing disk/mem and whatnot.

squashed_usr=[<arg>[,<arg>...]]
   Various parameters supported by the union_mount overlay.

   Ignored, see `union mount cmdline parameters`_ below

zram_swap[=<size_spec>]
   Specifies that the initramfs should set up zram swap block devices with
   a total size of ``<size_spec>`` (one per cpu core).

   ``<size_spec>`` is either a "fractional" relative to the max. system memory,
   e.g. "/2" => use halfth of the system memory as swap or a tmpfs-like
   size spec ("50%", "500m").

   When given without ``<size_spec>``, the size gets set as follows:

   * "/7" if sysmem >= 40000 MiB
   * "/6" if sysmem >= 30000 MiB
   * "/5" if sysmem >= 20000 MiB
   * "/4" if sysmem >= 10000 MiB
   * "/2" otherwise

zram_disk=<name>|name=<name>,[size=<size>]
   Allocates a zram block devices of the given size and formats it.

   [as ext2/ext4/btrfs, depending on config.h]

zdisk=...
   Alias to ``zram_disk``.

xshell[=never|fail|error|once|pre|preswitch|always]
   Specifies when (and if) */init* should run a shell:

   never
      Never run a shell. This is the default behavior.

   fail, error
      Run a shell when errors occur instead of immediately raising a kernel
      panic.

   once
      Run a user shell once (early),
      wait for it to exit and continue booting afterwards (unless non-zero return).

      Also enables ``xshell=fail`` behavior.

   pre, preswitch
      Run a user shell just before switching to */newroot*,
      wait for it to exit and continue booting afterwards (unless non-zero return).

      Also enables ``xshell=fail`` behavior.

   always
      Run a user shell loop (early).
      The boot process won't continue past this step.

stagedive=...
   Sets ``root`` to ``zroot`` and ``rootfstype`` to ``zdisk``.
   The actual *stagedive* implementation is up to the overlay.

liram=...
   Ignored.

liram_<option>=...
   Ignored.
