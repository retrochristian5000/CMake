deb-on-windows
--------------

* The :generator`DEB` generator was enabled on Windows.
  While not fully featured (due to the lack of external UNIX tools)
  this will allow building basic cross-platform debian packages.

  To communicate UNIX file permissions from the install stage
  to the CPack DEB generator the "cmake_mode_t" NTFS
  alternate data stream (ADT) is used.

  When a filesystem without ADT support is used only owner read/write
  permissions can be preserved.
