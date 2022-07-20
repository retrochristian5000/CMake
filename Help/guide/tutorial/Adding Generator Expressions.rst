Step 4: Adding Generator Expressions
=====================================

:manual:`Generator expressions <cmake-generator-expressions(7)>` are evaluated
during build system generation to produce information specific to each build
configuration.

:manual:`Generator expressions <cmake-generator-expressions(7)>` are allowed in
the context of many target properties, such as :prop_tgt:`LINK_LIBRARIES`,
:prop_tgt:`INCLUDE_DIRECTORIES`, :prop_tgt:`COMPILE_DEFINITIONS` and others.
They may also be used when using commands to populate those properties, such as
:command:`target_link_libraries`, :command:`target_include_directories`,
:command:`target_compile_definitions` and others.

:manual:`Generator expressions <cmake-generator-expressions(7)>`  may be used
to enable conditional linking, conditional definitions used when compiling,
conditional include directories and more. The conditions may be based on the
build configuration, target properties, platform information or any other
queryable information.

There are different types of
:manual:`generator expressions <cmake-generator-expressions(7)>` including
Logical, Informational, and Output expressions.

Logical expressions are used to create conditional output. The basic
expressions are the ``0`` and ``1`` expressions. A ``$<0:...>`` results in the
empty string, and ``<1:...>`` results in the content of ``...``.  They can also
be nested.

A common usage of
:manual:`generator expressions <cmake-generator-expressions(7)>` is to
conditionally add compiler flags, such as those for language levels or
warnings. A nice pattern is to associate this information to an ``INTERFACE``
target allowing this information to propagate. Let's start by constructing an
``INTERFACE`` target and specifying the required C++ standard level of ``11``
instead of using :variable:`CMAKE_CXX_STANDARD`.

So the following code:

.. literalinclude:: Step4/CMakeLists.txt
  :caption: CMakeLists.txt
  :name: CMakeLists.txt-CXX_STANDARD-variable-remove
  :language: cmake
  :start-after: project(Tutorial VERSION 1.0)
  :end-before: # should we use our own math functions

Would be replaced with:

.. literalinclude:: Step5/CMakeLists.txt
  :caption: CMakeLists.txt
  :name: CMakeLists.txt-cxx_std-feature
  :language: cmake
  :start-after: project(Tutorial VERSION 1.0)
  :end-before: # add compiler warning flags just when building this project via

**Note**:  This upcoming section will require a change to the
:command:`cmake_minimum_required` usage in the code.  The Generator Expression
that is about to be used was introduced in `3.15`.  Update the call to require
that more recent version:

.. code-block:: cmake
  :caption: CMakeLists.txt
  :name: CMakeLists.txt-version-update

  cmake_minimum_required(VERSION 3.15)

Next we add the desired compiler warning flags that we want for our project. As
warning flags vary based on the compiler we use the ``COMPILE_LANG_AND_ID``
generator expression to control which flags to apply given a language and a set
of compiler ids as seen below:

.. literalinclude:: Step5/CMakeLists.txt
  :caption: CMakeLists.txt
  :name: CMakeLists.txt-target_compile_options-genex
  :language: cmake
  :start-after: # the BUILD_INTERFACE genex
  :end-before: # should we use our own math functions

Looking at this we see that the warning flags are encapsulated inside a
``BUILD_INTERFACE`` condition. This is done so that consumers of our installed
project will not inherit our warning flags.

**Exercise**: Modify ``MathFunctions/CMakeLists.txt`` so that all targets have
a :command:`target_link_libraries` call to ``tutorial_compiler_flags``.

Next, let's use generator expressions to add code for exclusively debug mode.
When the build is run in release mode, all of this code will be removed. Add
the following line to the end of your MathFunctions CMakeList:

.. literalinclude:: Step5/MathFunctions/CMakeLists.txt
  :caption: CMakeLists.txt
  :name: CMakeLists.txt-target_compile_definitions-genex
  :language: cmake
  :start-after: # Creating a DEBUG variable to pass through

As you can see, generator expressions can be nested. The inner expression
checks if we are in config mode debug. This returns a boolean value. The
outer generator expression stores that boolean value in the variable DEBUG
for us to use in our code.

Since we have the variable DEBUG to use, lets move some our function print
outs to be a debug only feature. In mysqrt.cxx, wrap the print message with
``#ifdef DEBUG`` to look like the following.

.. literalinclude:: Step5/MathFunctions/mysqrt.cxx
  :caption: mysqrt.cxx
  :name: mysqrt.cxx-debug-ifdef
  :language: c++
  :start-after: result = result + 0.5 * delta / result;
  :end-before: }

With this all set, we can build in debug mode by calling the following
from the command line:

.. code-block:: console
  :caption: console
  :name: build-debug-config

  cmake --build . --config Debug

Then to build in release mode, we would call the following from the command
line:

.. code-block:: console
  :caption: console
  :name: build-release-config

  cmake --build . --config Release

Both of these commands will create the respective folders Debug and Release.
In these folders will be your executable. Run it to verify that the extended
output is only produced in debug mode.
