#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  // Note: GoogleTest.cmake doesn't actually depend on Google Test as such;
  // it only requires that we produces output in the expected format when
  // invoked with --gtest_list_tests. Thus, we fake that here. This allows us
  // to test the module without actually needing Google Test.
  if (argc > 2 && std::string(argv[1]) == "--gtest_list_tests" &&
      std::string(argv[2]).find("--gtest_output=json:") != std::string::npos) {
    std::cout << "configuration." << std::endl;
#ifdef DEBUG
    std::cout << "  DISABLED_case_release" << std::endl;
    std::cout << "  case_debug" << std::endl;
    std::string release_name("DISABLED_case_release");
    std::string debug_name("case_debug");
#else
    std::cout << "  case_release" << std::endl;
    std::cout << "  DISABLED_case_debug" << std::endl;
    std::string release_name("case_release");
    std::string debug_name("DISABLED_case_debug");
#endif

    if (!std::getenv("NO_GTEST_JSON_OUTPUT")) {
      std::string output_param(argv[2]);
      std::string::size_type split = output_param.find(":");
      std::string filepath = output_param.substr(split + 1);
      // The full file path is passed
      std::ofstream ostrm = std::ofstream(filepath.c_str(), std::ios::binary);
      if (!ostrm) {
        std::cerr << "Failed to create file: " << filepath.c_str()
                  << std::endl;
        return 1;
      }

      ostrm << R"({
    "tests": 2,
    "name": "AllTests",
    "testsuites": [
        {
            "name": "configuration",
            "tests": 2,
            "testsuite": [
                { "name": ")"
            << release_name << R"(", "file": "file.cpp", "line": 42 },
                { "name": ")"
            << debug_name << R"(", "file": "file.cpp", "line": 43 }
            ]
        }
    ]
})";
    }
    return 0;
  }

  return 1;
}
