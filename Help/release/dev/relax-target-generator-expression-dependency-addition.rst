relax-target-generator-expression-dependency-addition
-----------------------------------------------------

* The following target based generator expressions that query for directory or
  file name components don't construct a dependency on the evaluated target.

    - ``TARGET_FILE_DIR``
    - ``TARGET_LINKER_FILE_BASE_NAME``
    - ``TARGET_LINKER_FILE_NAME``
    - ``TARGET_LINKER_FILE_DIR``
    - ``TARGET_SONAME_FILE_NAME``
    - ``TARGET_SONAME_FILE_DIR``
    - ``TARGET_PDB_FILE_NAME``
    - ``TARGET_PDB_FILE_DIR``
    - ``TARGET_BUNDLE_DIR``
    - ``TARGET_BUNDLE_CONTENT_DIR``
