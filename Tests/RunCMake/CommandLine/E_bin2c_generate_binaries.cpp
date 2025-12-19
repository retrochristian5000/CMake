#include <cstdint>
#include <fstream>
#include <random>
#include <string>

int main(int argc, char** argv)
{
  std::string const basicTestNames[] = {
    "basic", "st", "static", "c", "const", "static_const", "n", "name",
  };
  char const basicContents[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
  };
  for (auto const& testName : basicTestNames) {
    std::ofstream fout((std::string("E_bin2c_") + testName + ".bin").c_str(),
                       std::ios::out | std::ios::binary);
    fout.write(basicContents, sizeof(basicContents));
    fout.close();
  }

  {
    std::ofstream fout("E_bin2c_empty.bin", std::ios::out | std::ios::binary);
    fout.close();
  }

  {
    std::ofstream fout("E_bin2c_text_lf.bin",
                       std::ios::out | std::ios::binary);
    fout << "All work and no play makes Jack a dull boy.\n";
    fout.close();
  }

  {
    std::ofstream fout("E_bin2c_text_crlf.bin",
                       std::ios::out | std::ios::binary);
    fout << "All work and no play makes Jack a dull boy.\r\n";
    fout.close();
  }

  {
    std::ofstream fout("E_bin2c_text_align.bin",
                       std::ios::out | std::ios::binary);
    fout << "This exactly 32 characters long!";
    fout.close();
  }

  {
    std::mt19937 engine(12345);
    std::ofstream fout("E_bin2c_very_long.bin",
                       std::ios::out | std::ios::binary);
    std::vector<std::uint8_t> buffer(65536);
    for (std::size_t i = 0; i < buffer.size(); i += sizeof(std::uint32_t)) {
      std::uint32_t result = engine();
      for (std::size_t j = 0; j < sizeof(std::uint32_t); j++) {
        buffer[i + j] = (result >> (j * 8)) & 0xFF;
      }
    }
    fout.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
    fout.close();
  }

  return 0;
}
