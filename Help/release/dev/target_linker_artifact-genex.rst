target_linker_artifact
--------------

* A new :genex:`TARGET_LINKER_ARTIFACT` was added to simplify retrieving
  lists of linking dependencies. Normal, imported, and interface libraries
  require querying different properties to retrieve the library location or
  linker option (`-lname`). This generator expression unifies access to this
  bit of information, accessing the :prop_tgt:`INTERFACE_LINK_LIBRARIES` or
  :prop_tgt:`LOCATION` property as required.
