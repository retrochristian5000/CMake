ExternalProject-declare-download-byproducts
-------------------------------------------

* The :module:`ExternalProject` module now declares ``BYPRODUCTS`` for the
  downloaded file for generated ``download`` steps. This may trigger errors in
  :ref:`Ninja Generators` where multiple projects download to the same file,
  however these projects already had hazards that if the download steps ran
  concurrently, the hash verification could fail because both download steps
  write directly to the same file path.
