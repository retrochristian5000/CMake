#include <cstdlib>
#include <iostream>
#include <string>

int runRealExe(int argc, char** argv)
{
  std::string cmd;
  const char* sep = "";
  for (int i = 0; i < argc; ++i) {
    cmd += sep;
    cmd += "\"";
    cmd += argv[i];
    cmd += "\"";
    sep = " ";
  }
  std::cout << "Running real exe: " << cmd << std::endl;
#ifdef _WIN32
  cmd = "cmd /C \"" + cmd + "\"";
#endif
  return std::system(cmd.c_str());
}

int main(int argc, char** argv)
{
  std::string cmdline;
  bool ownArgs = true;

  for (int i = 0; i < argc; i++) {
    std::string arg(argv[i]);
    std::cout << "test_launcher: got arg " << i << " '" << arg << "'"
              << std::endl;
    if (ownArgs && arg == "--") {
      ownArgs = false;
    } else if (!ownArgs) {
      return runRealExe(argc - i, argv + i);
    }
  }
  return 1;
}
