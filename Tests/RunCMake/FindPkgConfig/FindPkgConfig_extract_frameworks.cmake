find_package(PkgConfig)

set(foobar_LDFLAGS_OTHER "-Wl,xy -framework bar -Wl,other-framework -framework baz -Wl,abc -framework gosh -frameworkcrap -frame j -framework nix -Wl,def")
set(foobar_LIBRARIES "-lz -lm")

_pkgconfig_extract_frameworks("foobar")

if (NOT foobar_LDFLAGS_OTHER STREQUAL "-Wl,xy -Wl,other-framework -Wl,abc -frameworkcrap -frame j -Wl,def")
  message(SEND_ERROR "REMAINDER did not match: ${REMAINDER}")
endif ()
if (NOT foobar_LIBRARIES STREQUAL "-lz -lm -framework bar -framework baz -framework gosh -framework nix")
  message(SEND_ERROR "LIBS did not match: ${LIBS}")
endif ()
