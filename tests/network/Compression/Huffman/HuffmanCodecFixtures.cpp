#include "HuffmanCodecFixtures.hpp"
#include <cstddef>
#include <vector>
#include "CompressionConstants.hpp"
#include "CompressionTestConstants.hpp"
#include "HuffmanCodecTestConstants.hpp"
#include "HuffmanTable.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network::testing {

namespace {

std::size_t codedBitCountOfZeros(std::size_t size) {
  const HuffmanTable& table = serverToClientTable();
  return (size * table.codes[0].length) +
         table.codes[END_OF_BODY_SYMBOL].length;
}

}  // namespace

std::vector<std::byte> everyByteValue() {
  std::vector<std::byte> bytes;
  bytes.reserve(BYTE_VALUE_COUNT);
  for (std::size_t value = 0; value < BYTE_VALUE_COUNT; ++value) {
    bytes.push_back(static_cast<std::byte>(value));
  }
  return bytes;
}

std::vector<std::byte> bodyCodedWithPadding() {
  for (std::size_t size = 0; size < PADDING_SEARCH_LIMIT; ++size) {
    if (codedBitCountOfZeros(size) % BITS_PER_BYTE != 0) {
      return std::vector<std::byte>(size, std::byte{0});
    }
  }
  return {};
}

}  // namespace rtype::network::testing
