target-headers
--------------

* A new :command:`target_headers` command was added, which can be used to add
  headers to a target.
* New :prop_tgt:`HEADER_SETS` and :prop_tgt:`INTERFACE_HEADER_SETS` properties
  were added, which list the header sets associated with a target.
* New :prop_tgt:`HEADER_SET` and :prop_tgt:`HEADER_SET_<NAME>` properties were
  added, which list the headers in the associated header set.
* New :prop_tgt:`HEADER_DIR` and :prop_tgt:`HEADER_DIR_<NAME>` properties were
  added, which specify the base directory of the associated header set.
* The :command:`install(TARGETS)` command gained new ``HEADERS`` and
  ``HEADER_SET`` arguments, which can be used to install header sets associated
  with a target.
* The :manual:`File API <cmake-file-api(7)>` ``codemodel-v2`` minor version has
  been bumped to ``4``.
* The :manual:`File API <cmake-file-api(7)>` ``codemodel-v2`` ``directory``
  object gained a new installer type of ``headerSet``.
