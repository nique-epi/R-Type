#include "PackageDigest.hpp"
#include <openssl/evp.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include "Hashing.hpp"
#include "ReleaseConstants.hpp"

namespace rtype::client {

std::string sha256HexadecimalOf(std::span<const std::byte> bytes) {
  std::array<unsigned char, SHA256_BYTES> digest{};
  unsigned int written = 0;
  if (EVP_Digest(bytes.data(), bytes.size(), digest.data(), &written,
                 EVP_sha256(), nullptr) != 1 ||
      written != digest.size()) {
    return {};
  }
  std::array<std::byte, SHA256_BYTES> asBytes{};
  std::ranges::transform(digest, asBytes.begin(),
                         [](unsigned char value) { return std::byte{value}; });
  return network::hexadecimalOf(asBytes);
}

}  // namespace rtype::client
