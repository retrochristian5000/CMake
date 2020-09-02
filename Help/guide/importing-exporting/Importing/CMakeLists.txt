cmake_minimum_required(VERSION 3.15)
project(Importing)

# Add executable
add_executable(myexe IMPORTED)

# Set imported location
set_property(TARGET myexe PROPERTY
             IMPORTED_LOCATION "../InstallMyExe/bin/myexe")

# Add custom command to create source file
add_custom_command(OUTPUT main.cc COMMAND myexe)

# Use source file
add_executable(mynewexe main.cc)
