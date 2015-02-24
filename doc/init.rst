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

  * Mount the rootfs, /usr or /etc as a combination of squashfs files,
    tarballs, a disk and backing memory (tmpfs or zram)

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

Clone or update the git repository:

.. code::

   mkdir -p -- ~/git-src
   git clone git://github.com/dywisor/fischstaebchen.git ~/git-src/fischstaebchen


Likewise, for buildroot:

.. code::

   mkdir -p -- ~/git-src
   git clone git://git.buildroot.net/buildroot ~/git-src/buildroot


Build
=====

Create a configuration file:

   .. code::

      cd ~/git-src/buildroot

      make BR2_EXTERNAL=~/git-src/fischstaebchen/buildroot fischstaebchen_x86_64_defconfig

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

.. include:: frag/util_progs.rst

----
init
----

This section describes the boot process of the */init* program.


.. include:: frag/init_process.rst


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

.. include:: frag/cmdline_init.rst



.. _MISC OVERLAY CMDLINE PARAMETERS:

-------
overlay
-------

.. include:: frag/cmdline_overlay.rst



.. _UNION MOUNT CMDLINE PARAMETERS:

-----------
union mount
-----------

.. include:: frag/cmdline_union_mount.rst


.. _STAGEDIVE CMDLINE PARAMETERS:

---------
stagedive
---------

.. include:: frag/cmdline_stagedive.rst
