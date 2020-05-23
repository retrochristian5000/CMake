UNITY_BUILD_ANON_PREFIX
-----------------------

Anonymous namespaces present a problem for unity builds because
they are used to ensure that certain variables and declarations
are scoped to a translation unit which is approximated by a
single source file.

When source files are combined in a unity build file, those
variables in different files are combined in a single
translation unit and the names clash.

When this property is populated, and when :prop_tgt:`UNITY_BUILD`
is ``TRUE`` and when the generated unity file language is
``CXX``, the property value is used to generate macros to
assist the use of pseudononymous namespaces in unity builds.

The generated macros are:

* ``<prop_value>_NAMESPACE`` - A name unique to this file
  for psedudononymous namespaces
* ``<prop_value>_USING_NAMESPACE`` - A ``using namespace``
  declaration for the psedudononymous namespace
* ``<prop_value>(ARG)`` - Prefixes ``ARG`` with the namespace
  name

Given:

.. code-block:: cmake

  set_target_properties(myTarget PROPERTIES
    UNITY_BUILD "ON"
    UNITY_BUILD_ANON_PREFIX "MY_ANON"
  )

the ``MY_ANON_NAMESPACE`` macro is used to name namespaces
which would otherwise be anonymous. Either the ``MY_ANON``
macro or the ``MY_ANON_USING_NAMESPACE`` macro (according
to your coding style or preference) is used to scope the
local variables or to bring them into scope in particular
functions.

Source would then be written as:

.. code-block:: cpp

  #ifndef MYPROJECT_UNITY_BUILD
  // Compat for non-unity builds. Probably in your project config.h file
  #define MY_ANON_NAMESPACE
  #define MY_ANON_USING_NAMESPACE
  #define MY_ANON(ARG) ARG
  #endif

  namespace MY_ANON_NAMESPACE {
    // The name 'i' clashes (or could clash) with other
    // variables in other anonymous namespaces
    int i = 42;
  }

  int use_func_like_macro()
  {
    return MY_ANON(i);
  }

  int use_using_macro()
  {
    MY_ANON_USING_NAMESPACE;
    return i;
  }
