#include "ByteSequence.hpp"
#include <cstddef>
#include <initializer_list>
#include <vector>

namespace rtype::network::testing {

std::vector<std::byte> byteSequence(
    std::initializer_list<unsigned int> values) {
  std::vector<std::byte> bytes;
  bytes.reserve(values.size());
  for (const unsigned int value : values) {
    bytes.push_back(static_cast<std::byte>(value));
  }
  return bytes;
}

}  // namespace rtype::network::testing
