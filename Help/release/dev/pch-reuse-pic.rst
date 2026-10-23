pch-reuse-pic
-------------

* The :command:`target_precompile_headers` command's ``REUSE_FROM`` form
  now compiles all targets that share a precompiled header with consistent
  position-independent code flags, because the precompiled header records
  them.  Previously the flags could differ, e.g. an executable used the
  ``PIE`` flavor of the flags while the precompiled header was created with
  the ``PIC`` flavor, which Clang rejects.  See policy :policy:`CMP0227`.

  Such a target may therefore be compiled with position-independent code
  even though its own :prop_tgt:`POSITION_INDEPENDENT_CODE` target property
  is off, because the flags are enabled for the whole group as soon as one
  of its targets needs them.  Likewise, an executable of such a group is
  compiled with the ``PIC`` flavor of the flags rather than the ``PIE``
  flavor, which may generate slightly slower code, unless every target of
  the group is an executable.  Set :prop_tgt:`DISABLE_PRECOMPILE_HEADERS` on
  such a target, set :prop_sf:`SKIP_PRECOMPILE_HEADERS` on its sources, or do
  not share a precompiled header with it, to keep its own flags.
