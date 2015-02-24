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
