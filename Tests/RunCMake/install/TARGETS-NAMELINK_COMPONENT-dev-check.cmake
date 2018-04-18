set(_check_files
  [[lib]]
  [[lib/libnamelink-only\.(so|dylib)]]
  [[lib/libnamelink-sep\.(so|dylib)]]
  [[lib/libnamelink-uns-dev\.(so|dylib)]]
)
check_installed("^${_check_files}$")
