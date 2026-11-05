#include "CompressionFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <random>
#include <span>
#include <vector>
#include "CompressionTestConstants.hpp"

namespace rtype::network::testing {

std::vector<std::byte> bytesOf(std::span<const std::uint8_t> values) {
  std::vector<std::byte> bytes;
  bytes.reserve(values.size());
  for (const std::uint8_t value : values) {
    bytes.push_back(static_cast<std::byte>(value));
  }
  return bytes;
}

std::vector<std::byte> randomBytes(std::size_t size, std::uint32_t seed) {
  std::mt19937 generator{seed};
  std::uniform_int_distribution<std::uint32_t> distribution{0,
                                                            LARGEST_BYTE_VALUE};
  std::vector<std::byte> bytes;
  bytes.reserve(size);
  for (std::size_t index = 0; index < size; ++index) {
    bytes.push_back(static_cast<std::byte>(distribution(generator)));
  }
  return bytes;
}

}  // namespace rtype::network::testing
