find_package(ScopeReq CONFIG GLOBAL REQUIRED)
expect_global(ScopeReq::target TRUE)
expect_global(ScopeDep::target TRUE)
expect_property(
  ScopeReq::target INTERFACE_LINK_LIBRARIES
  "requirements" "ScopeDep::target")
