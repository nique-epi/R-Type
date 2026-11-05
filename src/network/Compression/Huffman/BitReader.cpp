#include "BitReader.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include "SerializationConstants.hpp"

namespace rtype::network {

BitReader::BitReader(std::span<const std::byte> bytes) : bytes_(bytes) {}

std::optional<std::uint8_t> BitReader::next() {
  if (position_ >= bytes_.size() * BITS_PER_BYTE) {
    return std::nullopt;
  }
  const std::uint8_t bit = bitAt(position_);
  ++position_;
  return bit;
}

bool BitReader::onlyZeroPaddingLeft() const {
  const std::size_t total = bytes_.size() * BITS_PER_BYTE;
  if (total - position_ >= BITS_PER_BYTE) {
    return false;
  }
  for (std::size_t position = position_; position < total; ++position) {
    if (bitAt(position) != 0) {
      return false;
    }
  }
  return true;
}

std::uint8_t BitReader::bitAt(std::size_t position) const {
  const auto byte = std::to_integer<unsigned>(bytes_[position / BITS_PER_BYTE]);
  const std::size_t shift = BITS_PER_BYTE - 1 - (position % BITS_PER_BYTE);
  return static_cast<std::uint8_t>((byte >> shift) & 1U);
}

}  // namespace rtype::network
