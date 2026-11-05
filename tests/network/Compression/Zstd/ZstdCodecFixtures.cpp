#include "ZstdCodecFixtures.hpp"
#include <cstddef>
#include <vector>
#include "CompressionFixtures.hpp"
#include "CompressionTestConstants.hpp"
#include "ZstdCodecTestConstants.hpp"

namespace rtype::network::testing {

std::vector<std::byte> everyByteValue() {
  std::vector<std::byte> bytes;
  bytes.reserve(BYTE_VALUE_COUNT);
  for (std::size_t value = 0; value < BYTE_VALUE_COUNT; ++value) {
    bytes.push_back(static_cast<std::byte>(value));
  }
  return bytes;
}

std::vector<std::byte> frameOfGarbage() {
  std::vector<std::byte> frame = bytesOf(FRAME_MAGIC_NUMBER);
  const std::vector<std::byte> garbage =
      randomBytes(GARBAGE_AFTER_THE_MAGIC_SIZE, BYTES_SEED);
  frame.insert(frame.end(), garbage.begin(), garbage.end());
  return frame;
}

}  // namespace rtype::network::testing
