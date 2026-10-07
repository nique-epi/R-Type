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

void ByteWriter::writeUint8(std::uint8_t value) {
  writeUnsigned(value, UINT8_WIRE_SIZE);
}

void ByteWriter::writeUint16(std::uint16_t value) {
  writeUnsigned(value, UINT16_WIRE_SIZE);
}

void ByteWriter::writeUint32(std::uint32_t value) {
  writeUnsigned(value, UINT32_WIRE_SIZE);
}

void ByteWriter::writeUint64(std::uint64_t value) {
  writeUnsigned(value, UINT64_WIRE_SIZE);
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

void ByteWriter::writeUnsigned(std::uint64_t value, std::size_t byteCount) {
  for (std::size_t index = byteCount; index > 0; --index) {
    const std::size_t shift = (index - 1) * BITS_PER_BYTE;
    bytes_.push_back(static_cast<std::byte>((value >> shift) & BYTE_MASK));
  }
}

}  // namespace rtype::network
