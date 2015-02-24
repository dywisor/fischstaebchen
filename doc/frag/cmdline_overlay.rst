aufs
   Prefer `AuFS3`_ for union mounts.

overlay, overlayfs
   Prefer `OverlayFS`_ for union mounts (linux >= 3.18).

   .. Note::

     overlayfs with more than two branches needs linux >= 3.20
     or >= 3.18 w/ backported multi-layer patch.
