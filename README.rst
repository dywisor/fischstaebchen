.. _TODO: /TODO

.. _issue tracker: https://github.com/dywisor/fischstaebchen/issues

.. _busybox: http://busybox.net/

.. _buildroot: http://buildroot.net/

.. _buildroot's documentation: http://buildroot.net/downloads/manual/manual.html

.. _aufs3: http://aufs.sourceforge.net/

.. _overlayfs: http://en.wikipedia.org/wiki/OverlayFS

.. _openbox: http://openbox.org/wiki/Main_Page

.. _Gentoo Linux: http://www.gentoo.org/

.. _Arch Linux: https://www.archlinux.org/

.. |NAME| replace:: Fischstaebchen
.. |LNAME| replace:: fischstaebchen



.. .. sectnum::

.. contents::
   :backlinks: entry
   :depth: 3


~~~WIP~~~

============
Introduction
============

An initramfs with emphasis on live systems and union mounts.


---------
Use Cases
---------

* boot a normal system with rootfs specified as UUID or LABEL

* mount squashed /usr (ro/rw), e.g.:

  * ``/usr.squashfs``

  * download a squashfs file via nfs/cifs/http, keep changes on a local disk

* *volatile rootfs* - mount a zram blockdev/tmpfs on top of the rootfs

* *stagedive* - diskless live system boot:

  download a rootfs image, set it up and boot it, includes support for:

  * `Gentoo Linux`_ stage3 [fetched from the official mirrors]

  * `Arch Linux`_ w/ `Openbox`_ (x86/x86_64 only) [fetched from the official mirrors]

  * generic GNU/Linux system


--------
Features
--------

* Can be built as buildroot package or standalone (*w/ reduced feature set*)

* The usual init thingy:

  * Mount disks via UUID/LABEL

  * Networking (DHCP only)

  * Rescue shell

  * Load kernel modules

* zram as swap and as tmpfs-like disk

* Union mounts (using aufs or overlayfs)

  * Mount the rootfs, /usr, /var and/or /etc
    as a combination of squashfs files, tarballs,
    a disk and backing memory (tmpfs or zram)

    .. Note::

      The stacking order is 0..1 tmpfs/zram mount on top of 0..1 disk on
      top of 0..N tarballs on top of 0..M squashfs files on top of 0..L
      "low-layer" tarballs.
      This is a limitation of the cmdline parser to keep the configuration
      rather simple. More complex setups have to be added manually.

  * Copy squashfs files to a tmpfs container and mount them from there

  * Get filesystem images via nfs/cifs/http or from disk


See `TODO`_ for a list of missing or incomplete features.


------
Design
------

The core functionality is written in C (GNU99),
specific tasks are offloaded to external programs (usually shell),
organized in phases.

Overall, the following programming Languages/Dialects are involved:

* C
* POSIX sh, awk, sed
* POSIX sh with "sed-able" variables (``@@VARNAME@@``)
* POSIX sh with preprocessor instructions (build-time only)
* Python (build-time only)


----
Bugs
----

If you happen to encounter a Gräte in |NAME|,
use the `issue tracker`_ to report it.


-------
License
-------

This project is distributed under the terms of the MIT license,
see LICENSE.MIT.


====================
Building from source
====================

|NAME| can be built as standalone image (with a few tools such as busybox)
or with `Buildroot`_, which is the far more powerful option.



-----------------------------
Building as buildroot package
-----------------------------

There's no package for this project in the official buildroot repo, you'll
have to use ``BR2_EXTERNAL``.


Download
========

Clone or update the *fischstaebchen-buildroot* git repository:

.. code::

   mkdir -p -- ~/git-src
   git clone git://github.com/dywisor/fischstaebchen-buildroot.git ~/git-src/fischstaebchen-buildroot


Likewise, for buildroot:

.. code::

   mkdir -p -- ~/git-src
   git clone git://git.buildroot.net/buildroot ~/git-src/buildroot


Build
=====

Create a configuration file:

   .. code::

      cd ~/git-src/buildroot

      make BR2_EXTERNAL=~/git-src/fischstaebchen-buildroot fischstaebchen_x86_64_defconfig

      make nconfig

Build it:

   .. code::

      make -C ~/git-src/buildroot


Refer to `buildroot's documentation`_ for details.



-------------------------------
Building a standalone initramfs
-------------------------------

Dependencies
============

Build dependencies:

   * C compiler (uclibc-based toolchain is recommended)

   * GNU make

   * python >= 2.7

   * git

   * internet access; the build process will download files, e.g. `Busybox`_


Download
========

Clone or update the git repository:

.. code::

   mkdir -p -- ~/git-src
   git clone git://github.com/dywisor/fischstaebchen.git ~/git-src/fischstaebchen


Build
=====

Change the working directory to ``<srcdir>/standalone`` and run ``make``:

.. code::

   make -C ~/git-src/fischstaebchen/standalone

Add ``FEATURES="mkfs"`` if you want ext4-formatted zram disks:

.. code::

   make -C ~/git-src/fischstaebchen/standalone FEATURES="mkfs"


Try ``X_FORCE_GOLD=1`` if building with ``clang`` fails.


Cross-Build
-----------

Simply pass ``CROSS_COMPILE`` to ``make``:

.. code::

   make -C ~/git-src/fischstaebchen/standalone CROSS_COMPILE=/my/toolchain/bin/<arch>-...


Out-Of-Tree Build
-----------------

Pass ``O=/some/where`` to ``make``.
The default output directory is ``<srcdir>/standalone/build``.


Install
=======

Copy ``boot.cpio``, ``boot.cpio.gz`` or ``boot.cpio.xz`` from the
output directory to your boot partition:

.. code::

   cd ~/git/src/fischstaebchen/standalone/build/boot.cpio.xz /boot/initfs.xz


and set up the bootloader accordingly, for example:

.. code::

   cat << EOF > /boot/extlinux/extlinux.conf
   UI /boot/extlinux/menu.c32
   PROMPT 0

   TIMEOUT 30
   DEFAULT linux

   MENU DEFAULT linux
   MENU TABMSG Press [TAB] to edit options
   MENU TITLE Boot Menu
   MENU CLEAR

   LABEL linux
      MENU LABEL ^1) linux
      LINUX      /boot/linux
      APPEND     quiet inoquiet zram.num_devices=12 zram_swap=/4 xshell=fail root=LABEL=rootfs squashed_usr=cam
      INITRD     /boot/initfs.xz

   LABEL gentoo-stage3
      MENU LABEL ^2) gentoo-stage3
      LINUX      /boot/linux
      APPEND     quiet inoquiet zram.num_devices=12 zram_swap=/4 xshell=fail stagedive=g aproyy=192.168.1.1:3142
      INITRD     /boot/initfs.xz
   EOF


---------------------------------
Building the helper programs only
---------------------------------

The ``init/`` subdirectory features a few utilities that can also be used
standalone (i.e., in normal systems without |NAME|'s init binary).
These programs can be built as *shared* (linked to lib\ |LNAME| and libc),
**non-shared** (linked to libc only) or **static** binaries (huge, esp w/ glibc):

.. code::

   cd  ~/git/src/fischstaebchen/init

   # build shared prog
   make <prog>

   # OR non-shared
   make NONSHARED=1 <prog>

   # OR static
   make STATIC=1 <prog>


   ## then, install:

   # shared only: install lib
   make DESTDIR=/ PREFIX=/usr/local install-libfischstaebchen

   make DESTDIR=/ PREFIX=/usr/local install-<prog>






========
Overview
========

---------------
helper programs
---------------

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
   The mode flags get *OR*\ed when more than one is specified.
   The default mode is ``-f``.

   The ``.`` suffix has the special meaning of "match basepath".

   Example Usage::

      filescan -f -- /mnt/disk/*/ -- docs.tar

      filescan -f -d -- /etc/ -- machine-id


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

   ``<num swaps>`` default to the cpu core count and the default ``<size>``
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


----
init
----

This section describes the boot process of the */init* program.


#. initramfs initalization

   #. basemounts

      * static */dev*
      * */proc*
      * */sys*
      * dynamic */dev* (*devtmpfs* or *tmpfs*, + essential device nodes)
      * */dev/pts*
      * */dev/shm*
      * */run*
      * */tmp*

   #. open the logfile

   #. run ldconfig (if necessary)

   #. read cmdline from

      * */cmdline.defaults*,
      * */proc/cmdline*,
      * */cmdline*,
      * */cmdline.extend* and
      * */cmdline.local*

      (read from all of the listed files, in the order as specified above)

      No processing done except for verbosity parameters (*quiet* etc.).

   #. baselayout (create dirs/symlinks in initramfs */*)

   #. install busybox applets in */busybox*

   #. set up mdev (register hotplug agent, run initial device scan)

   #. parse cmdline

   #. set up zram swap (if configured)

   #. run a user shell (if requested)

      (may exit or loop forever here)

   #. wait for disks (up to ``$rootfind`` centiseconds)

   #. create /run/initramfs

#. run **early-env-setup** hook

#. run **parse-cmdline** hook

#. run **env-setup** hook

#. load cmdline-specified kernel modules (if possible)

#. run **devfs-setup** hook

#. run **net-setup** hook

#. run **net-ifup** hook

#. run **post-net** hook

#. load cmdline-specified kernel modules (if not already done)

#. mount entries in / read from *initramfs-base*

#. set up /newroot

   #. run **pre-mount-newroot** hook

   #. mount entries in /mnt/aux read from *aux-early*

   #. mount /newroot

   #. run **mount-newroot** hook

   #. mount entries in /newroot read from *newroot-base*

   #. run **post-mount-newroot** hook

   #. mount entries in /mnt/aux read from *aux-base*

   #. run **subtree-mount** hook

   #. mount entries in /newroot read from *newroot-subtree*

   #. run **populate-newroot** hook

   #. mount temporary basemounts in */newroot*:

      * */dev*, */sys*, */proc* bind-mounted from initramfs */*
      * */dev/pts*, */dev/shm*

   #. mount entries in */newroot* read from ``<<some fstab file in /newroot>>``

   #. mount */newroot/tmp* (if enabled and not already done)

   #. create per-user/uid directories */newroot/tmp/users*

   #. run **setup-newroot** hook

   #. run **finalize-newroot** hook

#. unmount aux mounts (*/mnt/aux/\**)

#. run **net-ifdown** hook

#. run preswitch shell (if requested)

   Non-zero exit code aborts booting.

#. switch_root

   * unmount temporary newroot basemounts
   * unmount /run, /tmp, /dev/shm, /dev/pts
   * move  */dev*, */sys*, */proc* to */newroot* (or unmount)
   * ``exec switch_root /newroot`` -- done



*/init* catches failure of any of the steps listed above,
runs an on-error shell in that case (if ``xshell=fail`` specified on cmdline)
and unmounts most mounts before triggering a kernel panic ("attempted to kill init").


--------
file uri
--------

Path to a file.

The basic syntax for *file uris* is ``<type>=<basepath>/<filename>``
or (equivalent) ``<type>://<basepath>/<filename>``.
The following *types* are implemented:


file
   A local file. Can also be written as ``/<basepath>/<filename>``.

nfs
   ``nfs://192.168.1.1/export/file.dat``

cifs
   ``cifs://192.168.1.1/share/file.dat``

http
   ``http://192.168.1.1/file.dat``

https
   ``https://192.168.1.1/file.dat``

   Needs a buildroot-based initramfs.

github, gh
   ``gh://user/repo`` or ``gh://user/repo:ref``

   Expands to a github tarball *https* uri.

.. ftp
   (not tested)

.. disk
   (format/syntax not yet mature)

.. aux
   (_mostly_ internal use only)




==================
Cmdline Parameters
==================

----
init
----

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
   A hook should then take care of this instead.

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

xshell[=<when>[,<when>...]]
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



.. _MISC CMDLINE PARAMETERS:

----
misc
----

cpu_iucode[=<bool>]
   Enable or disable loading of CPU microcode.

   Enabled by default, but nonfatal (errors are ignored by the boot process).
   Provided by the ``misc`` hook overlay.

no_cpu_iucode
   Disable loading of CPU microcode.

xfer_fw[=<bool>]
   Enable or disable transferring of kernel firmware files from the initramfs
   to the rootfs.

   Enabled by default, but firmware files will only be copied if all of
   the following conditions are met:

   * ``/lib/firmware`` exists in the initramfs

   * ``/lib/firmware`` does not exist in the rootfs

   Which means that no files will be copied when booting a usual linux distro.

   Provided by the ``misc`` hook overlay.

xfer_kmod[=<bool>]
   Enable or disable transferring of kernel module files from the initramfs
   to the rootfs.

   Enabled by default, but kernel modules will only be copied if all of
   the following conditions are met:

   * the version ``kver`` of the booted kernel could be retrieved

   * ``/lib/modules/<kver>`` exists in the initramfs

   * ``/lib/modules/<kver>`` does not exist in the rootfs

   Provided by the ``misc`` hook overlay.


.. _MISC OVERLAY CMDLINE PARAMETERS:

-------
overlay
-------

aufs
   Prefer `AuFS3`_ for union mounts.

overlay, overlayfs
   Prefer `OverlayFS`_ for union mounts (linux >= 3.18).

   .. Note::

     overlayfs with more than two branches needs linux >= 3.20
     or >= 3.18 w/ backported multi-layer patch.




.. _UNION MOUNT CMDLINE PARAMETERS:

-----------
union mount
-----------

The ``squashed_usr``, ``var``, ``etc`` and ``volatile_rootfs``
cmdline parameters accept a number of comma-separated arguments:

   <file uri>, base=<file uri>
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



.. _STAGEDIVE CMDLINE PARAMETERS:

---------
stagedive
---------

apt_cacher_ng=<proxy>, aproxy=<proxy>
   Specify an ``apt-cacher-ng`` server.

   Highly recommended for ``stagedive-bootstrap`` mode.
   In fact, the default configuration doesn't allow you to bootstrap a
   system if this option is set.

dotfiles=<file uri>, dcfg=<file uri>
   Download ``<file uri>`` and unpack it to ``<rootfs>/factory/dotfiles/``.

   The file should be a tarball and should contain either a "tbt" file,
   or a ``user_install.sh`` and/or a ``system_install.sh`` script.

   The user/system files then get automatically installed to the rootfs
   by the *stagedive* setup scripts.

   May be specified more than once.
   An empty value unsets all previous declarations.

rootpass=<password>
   May be used to set the root password when stagediving.

   The default root password is **idkfa**.

setkmap=<keyboard layout>
   Keyboard layout, defaults to ``de-latin1-nodeadkeys``.

timezone=<timezone>, tz=<timezone>
   Timezone, defaults to ``Europe/Berlin``.

stagedive=[<type>,]["none"\|<name>\|[tarball=\|tb=\|squashfs=\|sfs=]<uri>]
   Live-boot a system from a tarball/squashfs file.

   Some *types* accept a ``<name>`` arg, which triggers a specialized
   bootstrap logic rather than the usual "download ``<uri>`` and add it as
   the lowest layer of the union mount" procedure.

   The ``none`` arg may be used to skip the rootfs image file setup
   and activate the setup scripts only.


   Types:
      gentoo
         A generic Gentoo-based system.

      gentoo-stage3, stage3, g
         An (official) Gentoo stage3 tarball.

         Accepts <name> arg of the following form:

            .. code::

               [<major arch>:]<arch>[-<variant>]

         Examples:

            * amd64

            * i686-hardened

            * arm:armv5tel

         If neither a <name> nor an <uri> is given, then a <name> gets
         automatically derived from ``uname -m``.
         This works for some architectures only, for example x86/x86_64,
         but not arm.


      gentoo-stage4, stage4, G
         A stage3-derived Gentoo system ("custom stage").

      archstrap
         Bootstrap(ped) Arch Linux.

         Creates a live user with name **arch** and password **arch**.
         Grants sudo permissions to the user (if applicable).

         Downloads the x86/x86_64 bootstrap image from the official mirrors.

         Accepts a ``<name>`` arg, which may be either ``x86_64`` or ``i686``
         and defaults to the machine's architecture.
         An ``<uri>`` should not be specified for this type,
         but is not forbidden.


      archstrap-openbox
         Bootstrap(ped) Arch Linux w/ Openbox desktop.
         See *archstrap* for details.

      archstrap-env
         Arch Linux bootstrap system. See *archstrap* for details.

      minimal
         Minimal setup - rootpass and essential directories/files.

      base
         ``minimal`` + complete baselayout

      default
         ``base`` + networking + openrc/systemd


stagedive_mask=
   Clears the stagedive setup hook mask.

stagedive_mask=<name>
   Disable a setup script,
   ``<name>`` may also contain wildcard chars, e.g. ``*hostname*``.

   Can be specified more than once.
