#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  // Note: GoogleTest.cmake doesn't actually depend on Google Test as such;
  // it only requires that we produces output in the expected format when
  // invoked with --gtest_list_tests. Thus, we fake that here. This allows us
  // to test the module without actually needing Google Test.
  if (argc > 1 && std::string(argv[1]) == "--gtest_list_tests") {
    std::cout << "test_list_test/test.\n";
    std::cout << "  case/0  # GetParam() = \"semicolon;\"\n";
    std::cout << "  case/1  # GetParam() = 'osb['\n";
    std::cout << "  case/2  # GetParam() = 'csb]'\n";
    std::cout << "  case/3  # GetParam() = 'S p a c e s'\n";
  }
  if (argc > 2 &&
      std::string(argv[2]).find("--gtest_output=json:") != std::string::npos) {
    std::string output_param(argv[2]);
    std::string::size_type split = output_param.find(":");
    std::string filepath = output_param.substr(split + 1);
    // The full file path is passed
    std::ofstream ostrm(filepath.c_str(), std::ios::binary);
    if (!ostrm) {
      std::cerr << "Failed to create file: " << filepath.c_str() << std::endl;
      return 1;
    }
    ostrm << R"({
    "tests": 4,
    "name": "AllTests",
    "testsuites": [
        {
            "name": "test_list_test/test",
            "tests": 4,
            "testsuite": [
                { "name": "case/0", "value_param": "\"semicolon;\"", "file": "file.cpp", "line": 42 },
                { "name": "case/1", "value_param": "'osb['", "file": "file.cpp", "line": 42 },
                { "name": "case/2", "value_param": "'csb]'", "file": "file.cpp", "line": 42 },
                { "name": "case/3", "value_param": "'S p a c e s'", "file": "file.cpp", "line": 42 }
            ]
        }
    ]
})";
  }
  return 0;
}
