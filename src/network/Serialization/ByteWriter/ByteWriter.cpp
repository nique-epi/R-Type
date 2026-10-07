#include "ByteWriter.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>
#include "NetworkException.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {

static_assert(sizeof(float) == UINT32_WIRE_SIZE &&
                  std::numeric_limits<float>::is_iec559,
              "float must be IEEE-754 binary32 to be sent as is");

namespace {

/**
 * @brief Appends an unsigned integer to a buffer, most significant byte
 * first. The number of bytes is the size of the integer type, so it cannot
 * disagree with the value.
 */
template <typename UnsignedInteger>
void appendBigEndian(std::vector<std::byte>& buffer, UnsignedInteger value) {
  for (std::size_t index = sizeof(UnsignedInteger); index > 0; --index) {
    const std::size_t shift = (index - 1) * BITS_PER_BYTE;
    const auto widened = static_cast<std::uint64_t>(value);
    buffer.push_back(static_cast<std::byte>((widened >> shift) & BYTE_MASK));
  }
}

}  // namespace

void ByteWriter::writeUint8(std::uint8_t value) {
  appendBigEndian(bytes_, value);
}

void ByteWriter::writeUint16(std::uint16_t value) {
  appendBigEndian(bytes_, value);
}

void ByteWriter::writeUint32(std::uint32_t value) {
  appendBigEndian(bytes_, value);
}

void ByteWriter::writeUint64(std::uint64_t value) {
  appendBigEndian(bytes_, value);
}

void ByteWriter::writeInt8(std::int8_t value) {
  writeUint8(static_cast<std::uint8_t>(value));
}

void ByteWriter::writeInt16(std::int16_t value) {
  writeUint16(static_cast<std::uint16_t>(value));
}

void ByteWriter::writeInt32(std::int32_t value) {
  writeUint32(static_cast<std::uint32_t>(value));
}

void ByteWriter::writeInt64(std::int64_t value) {
  writeUint64(static_cast<std::uint64_t>(value));
}

void ByteWriter::writeFloat(float value) {
  writeUint32(std::bit_cast<std::uint32_t>(value));
}

void ByteWriter::writeString(std::string_view value) {
  if (value.size() > MAX_SHORT_STRING_LENGTH) {
    throw StringTooLongException(value.size(), MAX_SHORT_STRING_LENGTH);
  }
  writeUint8(static_cast<std::uint8_t>(value.size()));
  for (const char character : value) {
    bytes_.push_back(static_cast<std::byte>(character));
  }
}

const std::vector<std::byte>& ByteWriter::bytes() const { return bytes_; }

}  // namespace rtype::network
