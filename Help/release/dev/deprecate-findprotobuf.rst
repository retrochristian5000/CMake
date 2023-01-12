deprecate-findprotobuf
----------------------

* The :module:`FindProtobuf` module is no longer used by the
  :command:`find_package` command as a find module.  This allows the Protobuf
  upstream to optionally provide its own ``ProtobufConfig.cmake`` package
  configuration file and have applications use it via
  ``find_package(Protobuf)`` rather than ``find_package(Protobuf CONFIG)``.
  See policy :policy:`CMP0144`.
