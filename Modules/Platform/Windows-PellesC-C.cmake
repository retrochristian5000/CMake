# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

include (Platform/Windows-PellesC)
__windows_compiler_pellesc(C)

# Enable Pelles C's Microsoft extensions by default when targeting Windows.
string(APPEND CMAKE_C_FLAGS_INIT " -Ze")
