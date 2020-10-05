
#include <algorithm>
#include <iostream>
#include <string>

#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#if defined(CRYPTOPP_NO_GLOBAL_BYTE)
using CryptoPP::byte;
#endif

int main()
{
  std::string message = "a";

  std::string known_digest_str =
    "ca978112ca1bbdcafac231b39a23dc4da786eff8147c4e72b9807785afee48bb";
  byte known_digest[CryptoPP::SHA256::DIGESTSIZE];
  CryptoPP::StringSource source(
    known_digest_str, true,
    new CryptoPP::HexDecoder(new CryptoPP::ArraySink(
      &known_digest[0], CryptoPP::SHA256::DIGESTSIZE)));

  byte digest[CryptoPP::SHA256::DIGESTSIZE];
  CryptoPP::SHA256().CalculateDigest(
    digest, reinterpret_cast<const byte*>(message.c_str()), message.length());

  return !std::equal(&known_digest[0],
                     &known_digest[CryptoPP::SHA256::DIGESTSIZE - 1],
                     &digest[0]);
}
