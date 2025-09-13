fileapi-direct-dependencies
---------------------------

* The :manual:`cmake-file-api(7)` "codemodel" version 2 now includes imported
  targets and all interface library targets in its replies.  Previously,
  imported targets were omitted, and only those interface targets that
  participated in the build system were included.  The "target" object gained
  ``imported``, ``local``, and ``abstract`` fields to support these additions.
  The ``type`` field can now also hold the value ``UNKNOWN_LIBRARY``.
