#include "BitWriter.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "HuffmanCode.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {

void BitWriter::write(HuffmanCode code) {
  for (std::size_t remaining = code.length; remaining > 0; --remaining) {
    appendBit((static_cast<unsigned>(code.bits) >> (remaining - 1)) & 1U);
  }
}

void BitWriter::appendBit(unsigned bit) {
  pending_ =
      static_cast<std::uint8_t>((static_cast<unsigned>(pending_) << 1U) | bit);
  ++pendingBitCount_;
  if (pendingBitCount_ == BITS_PER_BYTE) {
    bytes_.push_back(static_cast<std::byte>(pending_));
    pending_ = 0;
    pendingBitCount_ = 0;
  }
}

std::vector<std::byte> BitWriter::finish() {
  if (pendingBitCount_ > 0) {
    const auto filled = static_cast<std::uint8_t>(
        pending_ << (BITS_PER_BYTE - pendingBitCount_));
    bytes_.push_back(static_cast<std::byte>(filled));
    pending_ = 0;
    pendingBitCount_ = 0;
  }
  return std::move(bytes_);
}

}  // namespace rtype::network
