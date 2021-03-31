CMake Release Utilities
***********************

This directory contains scripts used to package CMake itself for distribution
on ``cmake.org``.  See also the `CMake Source Code Guide`_.

.. _`CMake Source Code Guide`: ../../Help/dev/source.rst

Docker
------

The ``<os>/<arch>/`` directories contain Docker specifications that anyone
may use to produce binaries for CMake on the following platforms:

* ``linux/x86_64/``: Linux on ``x86_64`` architectures.
* ``win/x86/``: Windows on ``x86_64`` and ``i386`` architectures.

Each ``<os>/<arch>/`` directory contains the following:

* ``<os>/<arch>/base/Dockerfile``:
  Produces a base image with a build environment for portable CMake binaries.
  This image is published in the `kitware/cmake Docker Hub Repository`_
  with tag ``build-<os>-<arch>-base-<date>``.

* ``<os>/<arch>/deps/Dockerfile``:
  Produces an image with custom-built dependencies for portable CMake binaries.
  This image is published in the `kitware/cmake Docker Hub Repository`_
  with tag ``build-<os>-<arch>-deps-<date>``.

* ``<os>/<arch>/Dockerfile``:
  Produce an image containing a portable CMake binary package.
  Build this image using the CMake source directory as the build context.
  The resulting image will have an ``/out`` (or ``c:/out``) directory
  containing the package.  For example, on Linux ``x86_64``:

  .. code-block:: console

    $ docker build --tag=cmake:build --network none \
        -f cmake-src/Utilities/Release/linux/x86_64/Dockerfile cmake-src
    $ docker container create --name cmake-build cmake:build
    $ docker cp cmake-build:/out .
    $ ls out/cmake-*-Linux-x86_64.*

  On Windows, the ``win/x86`` specifications support both the ``x86_64``
  and ``i386`` architectures selected via ``--build-arg ARCH=...``.

* ``<os>/<arch>/test/Dockerfile``:
  Produces a base image with a test environment for packaged CMake binaries.
  For example, on Linux ``x86_64``, one may build the test base image:

  .. code-block:: console

    $ docker build --tag=cmake:test-base \
        cmake-src/Utilities/Release/linux/x86_64/test

  Then create a local ``test/Dockerfile`` to prepare an image with both the
  CMake source tree and the above-built package::

    FROM cmake:test-base
    COPY cmake-src /opt/cmake/src/cmake
    ADD out/cmake-<ver>-Linux-x86_64.tar.gz /opt/
    ENV PATH=/opt/cmake-<ver>-Linux-x86_64/bin:$PATH

  Build the test image and run it to drive testing:

  .. code-block:: console

    $ docker build --tag cmake:test --network none -f test/Dockerfile .
    $ docker run --network none cmake:test bash test-make.bash
    $ docker run --network none cmake:test bash test-ninja.bash

  On Windows, the test scripts are called ``test-nmake.bat`` and
  ``test-ninja.bat``.  In the ``x86`` architecture they accept one
  argument specifying either ``x86_64`` or ``i386``.

.. _`kitware/cmake Docker Hub Repository`: https://hub.docker.com/r/kitware/cmake

macOS
-----

The ``macos/`` directory contains scripts used to produce dependencies
for building CMake binaries on macOS.

File Table v1
-------------

The set of package files disributed on ``cmake.org`` varies by CMake version.
One file, named ``cmake-<ver>-files-v1.json``, contains a table of the package
files available for a given version.  The format is a JSON object:

.. code-block:: json

  {
    "files": [
      {
        "os": ["...", "..."],
        "architecture": ["...", "..."],
        "name": "..."
      }
    ],
    "hashFiles": [
      {
        "algorithm": ["...", "..."],
        "name": "cmake-<version>-<algo>.txt",
        "signature": ["cmake-<version>-<algo>.txt.asc"]
      }
    ]
  }

The members are:

``files``
  A JSON array of entries corresponding to available package files.
  Each entry is a JSON object containing members:

  ``os``
    A JSON array of strings naming the operating system for which the
    package file is built, possibly using multiple alternative spellings,
    e.g. ``["macos", "macOS"]``.
    Source package files use the value ``["source"]``.

  ``architecture``
    A JSON array of strings naming the architecture(s) for which the
    package file is built, possibly using multiple alternative spellings,
    e.g. ``["arm64","x86_64"]`` for a macOS universal binary.
    Source package files use the value ``[]``.

  ``class``
    A JSON string naming the class of package.  The value is one of:

    ``archive``
      A tarball or zip archive.
      The extension, such as ``.tar.gz`` or ``.zip``, indicates the format.
      The rest of the file name matches the top-level directory in the archive.

    ``installer``
      An interactive installer.

    ``volume``
      A disk image (``.dmg`` on macOS).

  ``name``
    A JSON string specifying the name of the package file.

  ``macOSmin``
    Optional member that is present on package files for macOS.
    The value is a JSON string specifying the minimum version of macOS
    required to run the binary, e.g. ``"10.13"``.

``hashFiles``
  A JSON array of entries corresponding to files containing cryptographic
  hashes of the package file contents.  Each entry is a JSON object
  containing members:

  ``algorithm``
    A JSON array of strings naming a cryptographic hash algorithm, possibly
    using multiple alternative spellings, e.g. ``["sha256", "SHA-256"]``.

  ``name``
    A JSON string specifying the name of the file containing hashes,
    e.g. ``"cmake-<version>-SHA-256.txt"``.

  ``signature``
    A JSON array of strings naming files containing a cryptographic
    signature of the hash file specified by ``name``, e.g.
    ``["cmake-<version>-SHA-256.txt.asc"]``.

The table and hash files are generated by `files.bash`_ from
the `files-v1.json.in`_ template and the package files themselves.

.. _`files.bash`: files.bash
.. _`files-v1.json.in`: files-v1.json.in

File Table v1 Queries
^^^^^^^^^^^^^^^^^^^^^

Clients may download the `File Table v1`_ file ``cmake-<ver>-files-v1.json``
and query it to get the name(s) of specific package files adjacent to it.
Make queries as specific as possible in order to account for additional
alternative binaries in future CMake versions.

For example, one may use ``jq`` queries:

* To select a Windows binary archive supporting ``x86_64`` hosts::

    .files[] | select((.os[] | . == "windows") and
                      (.architecture[] | . == "x86_64") and
                      (.class == "archive")) | .name

* To select a Linux binary archive supporting ``aarch64`` hosts::

    .files[] | select((.os[] | . == "linux") and
                      (.architecture[] | . == "aarch64") and
                      (.class == "archive")) | .name

* To select a macOS binary archive supporting ``arm64`` hosts::

    .files[] | select((.os[] | . == "macos") and
                      (.architecture[] | . == "arm64") and
                      (.class == "archive")) | .name

* To select a macOS binary archive supporting macOS 10.12 on ``x86_64`` hosts::

    .files[] | select((.os[] | contains("macOS")) and
                      (.architecture[] | . == "x86_64") and
                      ([.macOSmin] | inside(["10.10", "10.11", "10.12"]))
                      ) | .name

* To select a SHA-256 hash file::

    .hashFiles[] | select(.algorithm[] | . == "SHA-256") | .name
