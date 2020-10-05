#[=======================================================================[.rst:
FindCyptoPP
--------------

Module for locating the Crypto++ encryption library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``CryptoPP::CryptoPP``
  The CryptoPP library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``CryptoPP_FOUND``
  True if the system has the Crypto++ library.
``CryptoPP_VERSION``
  The version of the Crypto++ library which was found.
``CryptoPP_INCLUDE_DIRS``
  Include directories needed to use Crypto++.
``CryptoPP_LIBRARIES``
  Libraries needed to link to Crypto++.


Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``CRYPTOPP_ROOT``
  Prefix for the installation of the Crypto++ library.

Hints
^^^^^

``CRYPTOPP_ROOT``
 If set as an environment variable, this is used as a hint to theprefix for the installation of the Crypto++ library.



#]=======================================================================]


find_path(CryptoPP_INCLUDE_DIR NAMES cryptopp/config.h DOC "CryptoPP include directory"
  HINTS ${CRYPTOPP_ROOT} ENV CRYPTOPP_ROOT
  PATH_SUFFIXES include)

find_library(CryptoPP_LIBRARY NAMES cryptopp DOC "CryptoPP library"
  HINTS ${CRYPTOPP_ROOT} ENV CRYPTOPP_ROOT
  PATH_SUFFIXES lib)

if(CryptoPP_INCLUDE_DIR)
    file(STRINGS ${CryptoPP_INCLUDE_DIR}/cryptopp/config.h _config_version REGEX "CRYPTOPP_VERSION")
    string(REGEX MATCH "([0-9])([0-9])([0-9])" _match_version ${_config_version})
    set(CryptoPP_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
    set(CryptoPP_VERSION_STRING ${CryptoPP_VERSION})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CryptoPP
    REQUIRED_VARS CryptoPP_INCLUDE_DIR CryptoPP_LIBRARY
    FOUND_VAR CryptoPP_FOUND
    VERSION_VAR CryptoPP_VERSION)

if(CryptoPP_FOUND AND NOT TARGET CryptoPP::CryptoPP)
    add_library(CryptoPP::CryptoPP UNKNOWN IMPORTED)
    set_target_properties(CryptoPP::CryptoPP PROPERTIES
        IMPORTED_LOCATION "${CryptoPP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CryptoPP_INCLUDE_DIR}")
endif()

mark_as_advanced(CryptoPP_INCLUDE_DIR CryptoPP_LIBRARY)
set(CryptoPP_INCLUDE_DIRS ${CryptoPP_INCLUDE_DIR})
set(CryptoPP_LIBRARIES ${CryptoPP_LIBRARY})
