FindGDAL-improvements
---------------------

The :module:`FindGDAL` module has been improved to document and mark as
advanced its cache variables. There is a new ``FindGDAL_SKIP_GDAL_CONFIG``
variable which may be used to skip over the ``gdal-config``-based search.
Users may also set ``GDAL_ADDITIONAL_LIBRARY_VERSIONS`` to add additional
versions to the library name search strategy.
