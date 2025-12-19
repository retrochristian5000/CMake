enable_language(CXX)

add_executable(E_bin2c_generate_binaries E_bin2c_generate_binaries.cpp)
target_compile_features(E_bin2c_generate_binaries PRIVATE cxx_std_11)
add_custom_command(
  TARGET E_bin2c_generate_binaries POST_BUILD
  COMMAND E_bin2c_generate_binaries
  )
