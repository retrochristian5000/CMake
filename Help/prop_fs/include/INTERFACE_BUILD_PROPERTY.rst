List of public |property_name| requirements for a file set.

File sets may populate this property to publish the |property_name|
required to compile against the sources for the target.  The |command_name|
command populates this property.

When target dependencies are specified using :command:`target_link_libraries`,
CMake will read this property from file sets of all target dependencies to
determine the build properties of the consumer.

Contents of |PROPERTY_INTERFACE_NAME| may use "generator expressions"
with the syntax ``$<...>``.  See the :manual:`cmake-generator-expressions(7)`
manual for available expressions.  See the :manual:`cmake-buildsystem(7)`
-manual for more on defining buildsystem properties.
