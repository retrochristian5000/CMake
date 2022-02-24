VERIFY_HEADER_SETS
------------------

.. versionadded:: 3.24

Used to verify that all headers in a target's header sets can be included on
their own.

When this property is set to true, and the target is an object library, static
library, shared library, or executable with exports enabled, and the target
has one or more header sets, an object library target named
``<target_name>_verify_header_sets`` is created. This verification target has
one source file per header in the header sets whose :prop_sf:`LANGUAGE`
property is set. Headers whose :prop_sf:`LANGUAGE` property is not set are not
checked. Each source file only includes its associated header file. The
verification target links against the original target to get all of its usage
requirements. The verification target has its :prop_tgt:`EXCLUDE_FROM_ALL`
property set to true, and its :prop_tgt:`AUTOMOC`, :prop_tgt:`AUTORCC`,
:prop_tgt:`AUTOUIC`, and :prop_tgt:`UNITY_BUILD` properties set to false.
