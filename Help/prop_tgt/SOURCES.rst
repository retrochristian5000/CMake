SOURCES
-------

List of sources to compile a target.

This specifies the sources that take part in a target's compilation.
The :command:`target_sources` command may be used to populate this property.

Relative paths are evaluated with respect to the target's
:prop_tgt:`SOURCE_DIR` or :prop_tgt:`BINARY_DIR` properties
when one of the paths corresponds to:

* a :prop_sf:`GENERATED` source file, or
* an existing file on the file system.

Contents of ``SOURCES`` may use "generator expressions".
See the :manual:`cmake-generator-expressions(7)`
manual for available expressions.  See the :manual:`cmake-buildsystem(7)`
manual for more on defining buildsystem properties.
