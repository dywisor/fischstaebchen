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

      * */cmdline/defaults*,
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
